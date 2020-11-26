package main 
		 
func parneneparne(cislo int){
	print("\nfunkcia zistuje ci je cislo parne alebo neparne \n")
	n:=cislo
	print("bolo zadane cislo: ",n, "\n")
	if n/2==0 {
        print("cislo ", n, " je parne", "\n")
	} else{
        print("cislo ",n, " je neparne", "\n")
	}
}

func fibonacci(n int){
    print("\nfunkcia Fibonacci \n")
    t1:=0
    t2:=1
    nextTerm:=0
    print("Fibbonaciho postupnost pre zadane cislo ",n, " je :")
	for i:=1;i<=n;i = i+1 {
        if i==1 {
            print(" ",t1)
        } else {
        }
        if i==2 {
            print(" ",t2)
        } else {
        }
        nextTerm = t1 + t2
        t1=t2
        t2=nextTerm
        print(" ",nextTerm)
	}
	print("\n")
}

func vymenacisel(){
    print("\nfunkcia vymenacisel \n")
    first:= 5
	second:=10
	print("Prve cislo je :",first,"\n")
    print("Druhe cislo je :",second,"\n")
    first = first-second
    second = first+second
	first = second-first
	print("prebehla vymena cisel \n")
    print("Prve cislo je :",first,"\n")
	print("Druhe cislo je :",second,"\n")
}

func porovnaniestringov(retazec1 string,retazec2 string){
    print("\nfunkcia porovnanie stringov \n")
    output:= Compare(retazec1,retazec2)
    if output==0 {
      print("retazce sa rovnaju \n ")
    } else{
     print("retazce sa nerovnaju \n" )
      }
}

func main(){
	print("Hello World \n")
	cislo:=11
	parneneparne(cislo)
	n:= 5
	fibonacci(n) 
	vymenacisel()
	retazec1:="pes"
	retazec2:="macka"
	porovnaniestringov(retazec1,retazec2)	
}
 