default:
	cd makeprocs; make
	cd test2_1; make
	cd test2_2; make
	cd test2_3; make
	cd test2_4; make
	cd test2_6; make

clean:
	cd makeprocs; make clean
	cd test2_1; make clean
	cd test2_2; make clean
	cd test2_3; make clean
	cd test2_4; make clean
	cd test2_6; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 6; ee469_fixterminal

