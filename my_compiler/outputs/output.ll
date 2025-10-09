; ModuleID = 'cmini'
source_filename = "cmini"

define i32 @add(i32 %a, i32 %b) {
entry:
  ; fallback param a
  ; fallback param b
  %t1 = add i32 %a, %b
  ret i32 %t1
}

define i32 @main() {
entry:
  %t2 = alloca i32
  ; map x -> %t2
  store i32 10, i32* %t2
  %t3 = alloca i32
  ; map y -> %t3
  store i32 20, i32* %t3
  %t4 = alloca i32
  ; map z -> %t4
  ; load from alloca of x
  %t5 = load i32, i32* %t2
  ; load from alloca of y
  %t6 = load i32, i32* %t3
  %t7 = add i32 %t5, %t6
  store i32 %t7, i32* %t4
  ; load from alloca of z
  %t8 = load i32, i32* %t4
  ret i32 %t8
}

