secao texto
copy zero, older
copy one, old
input limit
output old
front: load older
add old
store new
sub limit
jmpp final
output new
copy old, older
copy new, old
jmp front
final: output limit
stop
secao dados
zero: const 0
one: const 1
older: space
old: space
new: space
limit: space
