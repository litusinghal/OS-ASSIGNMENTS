int fib(int n) {
  if(n<2) return n;
  else return fib(n-1)+fib(n-2);
}

int _start() {
	int val = fib(40);
	return val;
}
// int _start(){
//   int number =109;
//   int prime =1;
//   for(int i =2;i<number ;i++){
// if((number%i)==0 ) prime =0;
//   }
//   return prime ==1? number : -1;
// }