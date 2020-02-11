default:
	cd makeprocs; make
	cd inject_H2O; make
	cd inject_SO4; make
	cd react_2H2O; make
	cd react_SO4; make
	cd react_H2_O2_SO2; make
clean:
	cd makeprocs; make clean
	cd inject_H2O; make clean
	cd inject_SO4; make clean
	cd react_2H2O; make clean
	cd react_SO4; make clean
	cd react_H2_O2_SO2; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3 3; ee469_fixterminal
