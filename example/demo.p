const limit=10;
var n, f, test, t1, t2;
begin
     n := 0;
     f := 1;
     while n # limit do
     begin
          n := n + 1;
          f := f * n
     end;
     test := 1+2-3*4/(5-6)+7;
	t1:=test*2;
	t2:=t1+test;
     write n, f, test, t1, t2
end.

