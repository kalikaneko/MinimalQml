package main

/* a minimal example of how to register C callbacks and calling them from go */

import (
	"bytes"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"reflect"
	"sync"
	"unsafe"
)

// typedef void (*cb)();
// static void _do_callback(cb f) {
// 	f();
// }
import "C"

/* callbacks into C-land */

var mut sync.Mutex
var stmut sync.Mutex
var cbs = make(map[string](*[0]byte))
var initOnce sync.Once

// Events knows about all the posible events that C functions can be interested
// in subscribing to. You cannot subscribe to an event that is not listed here.
// FIXME use iota, and map from string?
type Events struct {
	OnStatusChanged string
}

// subscribe registers a callback from C-land.
// This callback needs to be passed as a void* C function pointer.
func subscribe(event string, fp unsafe.Pointer) {
	mut.Lock()
	defer mut.Unlock()
	e := &Events{}
	v := reflect.Indirect(reflect.ValueOf(&e))
	hf := v.Elem().FieldByName(event)
	if reflect.ValueOf(hf).IsZero() {
		fmt.Println("ERROR: not a valid event:", event)
	} else {
		cbs[event] = (*[0]byte)(fp)
	}
}

// trigger fires a callback from C-land.
func trigger(event string) {
	mut.Lock()
	defer mut.Unlock()
	cb := cbs[event]
	if cb != nil {
		C._do_callback(cb)
	} else {
		fmt.Println("ERROR: this event does not have subscribers:", event)
	}
}

/* connection status */

// status reflects the current state of the VPN connection. Go code is responsible for
// updating it; C-land just watches its changes and pulls its updates via the serialized
// context object.
type status int

const (
	off status = iota
	connecting
	on
	disconnecting
	failed
)

func (s status) String() string {
	return [...]string{"off", "connecting", "on", "disconnecting", "failed"}[s]
}

func (s status) MarshalJSON() ([]byte, error) {
	b := bytes.NewBufferString(`"`)
	b.WriteString(s.String())
	b.WriteString(`"`)
	return b.Bytes(), nil
}

// An action is originated in the UI. They are requests coming from the
// frontend via the C code. VPN code needs to watch them and fullfill their
// requests as soon as possible.
type actions int

const (
	switchOn actions = iota
	switchOff
	unblock
)

func (a actions) String() string {
	return [...]string{"switchOn", "switchOff", "unblock"}[a]
}

func (a actions) MarshalJSON() ([]byte, error) {
	b := bytes.NewBufferString(`"`)
	b.WriteString(a.String())
	b.WriteString(`"`)
	return b.Bytes(), nil
}

// connectionCtx is the global struct that is passed around to C-land. It also
// serves as the primary way of passing requests from the frontend to the Go-core, by
// letting the UI write some of these variables and processing them.
type connectionCtx struct {
	AppName  string    `json:"appName"`
	Provider string    `json:"provider"`
	Status   status    `json:"status"`
	Actions  []actions `json:"actions,omitempty"`
}

func (c connectionCtx) toJson() ([]byte, error) {
	stmut.Lock()
	defer stmut.Unlock()
	b, err := json.Marshal(c)
	if err != nil {
		log.Println(err)
		return nil, err
	}
	return b, nil
}

var ctx *connectionCtx

func setStatus(st status) {
	stmut.Lock()
	defer stmut.Unlock()
	ctx.Status = st
	go trigger("OnStatusChanged")
}

// initializeContext initializes an empty connStatus and sets the global
// ctx variable to it. This is expected to be called only once, so the public
// api uses the sync.Once primitive to call this.
func initializeContext(provider, appName string) {
	var st status = off
	ctx = &connectionCtx{
		AppName:  appName,
		Provider: provider,
		Status:   st,
	}
}

/* mock http server for mocking vpn behavior on ui interaction */

func mockUIOn(w http.ResponseWriter, r *http.Request) {
	log.Println("changing status: on")
	var st status = on
	setStatus(st)
}

func mockUIOff(w http.ResponseWriter, r *http.Request) {
	log.Println("changing status: off")
	var st status = off
	setStatus(st)
}

func mockUI() {
	http.HandleFunc("/on", mockUIOn)
	http.HandleFunc("/off", mockUIOff)
	http.ListenAndServe(":8080", nil)
}

/*

  exported C api

*/

//export SayHello
func SayHello() {
	fmt.Println("hello from go, earthling!")
}

//export TriggerStatusChange
func TriggerStatusChange() {
	// XXX just for testing
	trigger("OnStatusChanged")
}

//export SubscribeToEvent
func SubscribeToEvent(event string, f unsafe.Pointer) {
	subscribe(event, f)
}

//export InitializeContext
func InitializeContext() {
	provider := "black.riseup.net"
	appName := "RiseupVPN"
	initOnce.Do(func() {
		initializeContext(provider, appName)
	})
	fmt.Println(">>> ctx: ", ctx)
	c, _ := ctx.toJson()
	fmt.Println(">>> ctx: ", string(c))
}

//export MockUIInteraction
func MockUIInteraction() {
	log.Println("mocking ui interaction on port 8080")
	go mockUI()
}

//export RefreshContext
func RefreshContext() *C.char {
	log.Println(">> refreshing ctx")
	c, _ := ctx.toJson()
	return C.CString(string(c))
}

func main() {}

/*
TODO:
  [x] modify state (struct?) from outside
  [x] setState function that calls C callback
  [x] serialize the whole context struct into Json
  [ ] receive the json in c++
  [ ] update the Qml model
  [ ] see that the UI reflects change
  [ ] call Go Functions from Qml/c++ (switch off / on)
  [ ] trigger ui events (like dialogs) from Go-land
*/
