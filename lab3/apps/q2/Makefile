default:
	cd makeprocs; make
	cd inj_CO; make
	cd inj_S2; make
	cd re_SO4; make
	cd re_CO; make
	cd re_S2; make

clean:
	cd makeprocs; make clean
	cd inj_CO; make clean
	cd inj_S2; make clean
	cd re_SO4; make clean
	cd re_CO; make clean
	cd re_S2; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 1 4; ee469_fixterminal
	##cd ../../bin; dlxsim -x os.dlx.obj -a -D -u makeprocs.dlx.obj 1 4; ee469_fixterminal
