my_str = "Monday!Tuesday!Wednesday"
days = my_str.split('!')
print(days)
print(days[0])
print(days[-1].upper())

firsts = [d[0].upper() for d in days]
print(firsts)


squares = [idx*idx for idx in range(3)]
print(f"squares[-1] is squares[-1]" )

cols = ["violet","lilac","purple"]
together = list(zip(squares,cols))
print(together)

set_2 = {True,False,True, False}
print( set_2 ) 
tuple1 = (1,2,3)
print( tuple1 )
tuple2 = (2,22,222)
print( tuple2[-1] )
