default:
	cd makeprocs; make
	cd tester; make

clean:
	cd makeprocs; make clean
	cd tester; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj; ee469_fixterminal

