build: golib
	gcc -fpermissive -pthread main.cpp wrapper.a -o wrapper
golib:
	go build -buildmode=c-archive wrapper.go
clean:
	rm -f wrapper.h wrapper.a wrapper
