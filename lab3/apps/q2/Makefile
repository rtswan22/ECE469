default:
	cd makeprocs; make
	cd inj_CO; make
	cd inj_S2; make
	cd react_SO4; make
	cd sp_CO; make
	cd sp_S2; make

clean:
	cd makeprocs; make clean
	cd inj_CO; make clean
	cd inj_S2; make clean
	cd react_SO4; make clean
	cd sp_CO; make clean
	cd sp_S2; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 1 4; ee469_fixterminal
