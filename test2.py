months = ['January', 'February', 'March']
lens = [len(m) for m in months]
print( lens )

mos = [m[:3] for m in months]
together = list(zip(mos,lens))
print(together)

new_tog = sorted(together,\
    key = lambda elt: elt[1])
print(new_tog)

squares = [idx*idx for idx in range(3)]
print(f"squares[-1] is squares[-1]" )
print(f"squares[-1] is {squares[-1]}" ) 

translate = {"one":1,"two":2}
translate["ten"] = 10
print(translate.keys()) 