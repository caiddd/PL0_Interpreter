var x, y, n;

procedure fib;
var tmp;
begin
    while n # 0 do
    begin
        write y;
        tmp := y;
        y := y + x;
        x := tmp;
        n := n - 1
    end
end;

begin
    x := 0; 
    y := 1;
    n := 10;
    call fib
end.