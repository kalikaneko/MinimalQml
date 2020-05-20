package main	// buildmode=[c-archive|c-shared] requires exactly one main package

import "C"
import "fmt"

//export hello
func hello(name string) {
        fmt.Printf("Hello from Go, %s!\n", name);
}

func main() {
        fmt.Println("Initializing Go...") 
}
