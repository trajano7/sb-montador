load space
input space
input old_data
load old_data
l1: div dois
store new_data
mult dois
store tmp_data
load old_data
sub tmp_data
store tmp_data
output tmp_data
copy new_data, old_data
load old_data
jmpp 1
stop
dois: const 10
old_data: space
new_data: space
tmp_data: space
