/*int a,b,c,d,e,f,g,h;
void func4() {
	for (;e;)
	{	for(;d;);	}
}

void func3() {
	for(;b;){
		for(;d;) { func4(); }
	}
}

void func2() {
	func3();
	for(;c;);
}

void func1() {
	func2();
		for(;a;);
}

int main()
{
	for ( ;f;)	{
		for (;g;) {
			func4();
			func1();
		}
	}
	for ( ;h;) {
		func4();
		func1();
	}
	return 0;
}
*/
int main()
{
	int a = 9;
	int b = 9;
	int c = 9;

	int d = ((b*a) + (1+c)) + c;
	int e = ( a + c)*d;
	int f = c * a;

	d = c * e;
	e = c + f;
	f = (a+ c)*e;

	for (;d;)
	{
		for(;e;)
		{
			for(;f;);
		}
	}


	return 0;
}
