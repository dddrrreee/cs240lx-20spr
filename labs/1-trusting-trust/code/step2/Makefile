# you should not need to modify this.
CC =gcc
CFLAGS = -Og -g -Wall -Wno-unused-variable

all: login identity-cc identity-cc.check trojan-cc1

login: login.c
	$(CC) $(CFLAGS) $< -o $@

identity-cc: identity-cc.c
	$(CC) $(CFLAGS) $< -o $@

trojan-cc1: trojan-cc1.c
	$(CC) $(CFLAGS) $< -o $@

identity-cc.check:
	@echo "------------------------------------------";
	@echo "going to check that identity-cc compiles login";
	./identity-cc login.c -o login
	echo "guest\npassword\n" | ./login
	@echo "guest\npassword\n" | ./login > out
	diff login.out out
	@echo "success: identity-cc compiled login correctly!"
	@echo "------------------------------------------";

clean:
	rm -f  cc-attacked login-attacked out login identity-cc trojan-cc1 *.o a.out  *~ cc-attacked.c login-attacked.c out.c temp-out.c temp-out.c
