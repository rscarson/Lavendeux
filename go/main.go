package main

func main() {
	e := "sqrt(2)"
	for len(e) > 0 {
		t := nextToken(&e)
		print(t.v)
		print(" -> ")
		print(t.t)
		print("\n")
	}
}