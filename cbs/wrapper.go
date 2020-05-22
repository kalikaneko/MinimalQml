package main

/* a minimal example of how to register C callbacks and calling them from go */

import (
	"fmt"
	"reflect"
	"sync"
	"unsafe"
)

// typedef void (*cb)();
// static void _do_callback(cb f) {
// 	f();
// }
import "C"

type Hooks struct {
	OnThingA string
	OnThingB string
}

var mut sync.Mutex
var cbs = make(map[string](*[0]byte))

func registerCb(name string, fp unsafe.Pointer) {
	mut.Lock()
	defer mut.Unlock()
	h := &Hooks{}
	v := reflect.Indirect(reflect.ValueOf(&h))
	hf := v.Elem().FieldByName(name)
	if reflect.ValueOf(hf).IsZero() {
		fmt.Println("ERROR: not a valid hook:", name)
	} else {
		cbs[name] = (*[0]byte)(fp)
	}
}

func fireCb(name string) {
	mut.Lock()
	defer mut.Unlock()
	cb := cbs[name]
	if cb != nil {
		C._do_callback(cb)
	} else {
		fmt.Println("ERROR: callback not set", name)
	}
}

//export RegisterCallbacks
func RegisterCallbacks(f1, f2, f3 unsafe.Pointer) {
	fmt.Println("passed args", f1, f2, f3)

	registerCb("OnThingA", f1)
	registerCb("OnThingB", f2)
	registerCb("OnThingX", f1)

	fireCb("OnThingA")
	fireCb("OnThingB")
	fireCb("OnThingX")

	fmt.Println("done")
}

//export SayHello
func SayHello() {
	fmt.Println("hello from go")
}

func main() {}
