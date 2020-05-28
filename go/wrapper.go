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
	"time"
	"unsafe"
)

// typedef void (*cb)();
// inline void _do_callback(cb f) {
// 	f();
// }
import "C"

/* callbacks into C-land */

var mut sync.Mutex
var stmut sync.Mutex
var cbs = make(map[string](*[0]byte))
var initOnce sync.Once

// Events are just a enumeration of all the posible events that C functions can
// be interested in subscribing to. You cannot subscribe to an event that is
// not listed here.
type Events struct {
	OnStatusChanged string
}

const OnStatusChanged string = "OnStatusChanged"

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

// status reflects the current VPN status. Go code is responsible for updating
// it; C-land just watches its changes and pulls its updates via the serialized
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

// An action is originated in the UI. These represent requests coming from the
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

// The connectionCtx keeps the global state that is passed around to C-land. It
// also serves as the primary way of passing requests from the frontend to the
// Go-core, by letting the UI write some of these variables and processing
// them.
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
	go trigger(OnStatusChanged)
}

// initializeContext initializes an empty connStatus and assigns it to the
// global ctx holder. This is expected to be called only once, so the public
// api uses the sync.Once primitive to call this.
func initializeContext(provider, appName string) {
	var st status = off
	ctx = &connectionCtx{
		AppName:  appName,
		Provider: provider,
		Status:   st,
	}
	go trigger(OnStatusChanged)
}

/* mock http server: easy way to mocking vpn behavior on ui interaction. This
* should also show a good way of writing functionality tests just for the Qml
* layer */

func mockUIOn(w http.ResponseWriter, r *http.Request) {
	log.Println("changing status: on")
	setStatus(on)
}

func mockUIOff(w http.ResponseWriter, r *http.Request) {
	log.Println("changing status: off")
	setStatus(off)
}

func mockUIFailed(w http.ResponseWriter, r *http.Request) {
	log.Println("changing status: failed")
	setStatus(failed)
}

func mockUI() {
	http.HandleFunc("/on", mockUIOn)
	http.HandleFunc("/off", mockUIOff)
	http.HandleFunc("/failed", mockUIFailed)
	http.ListenAndServe(":8080", nil)
}

/*

  exported C api

*/

//export SwitchOn
func SwitchOn() {
	fmt.Println("switch on...")
	setStatus(connecting)
	go func() {
		time.Sleep(2 * time.Second)
		setStatus(on)
	}()
}

//export SwitchOff
func SwitchOff() {
	fmt.Println("switch off...")
	setStatus(disconnecting)
	go func() {
		time.Sleep(2 * time.Second)
		setStatus(off)
	}()
}

//export Unblock
func Unblock() {
	fmt.Println("unblock... [not implemented]")
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
}

//export RefreshContext
func RefreshContext() *C.char {
	c, _ := ctx.toJson()
	return C.CString(string(c))
}

/* end of the exposed api */

/* we could enable this one optionally for the qt tests */

//export MockUIInteraction
func MockUIInteraction() {
	log.Println("mocking ui interaction on port 8080. \nTry 'curl localhost:8080/{on|off|failed}' to toggle status.")
	go mockUI()
}

func main() {}
