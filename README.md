# Pyforce
Pyforce is a custom package for [LAMMPS](http://lammps.sandia.gov), adding a fix command for calculating forces with Python.

**Author**: Jinzhe Zeng

**Email**: jzzeng@stu.ecnu.edu.cn

## Complile

Download LAMMPS first, then

```bash
git clone https://github.com/njzjz/Pyforce.git
cp -r Pyforce/PYFORCE/ $LAMMPS_DIR/src
cd $LAMMPS_DIR/src
make yes-pyforce
make serial
```

## Usage
Add a line in LAMMPS input file:
```
fix 1 all pyforce C H O
```

Also, a Python program `force.py`, which reads the XYZ format file `comb.xyz` and writes forces `forces.dat`, should be put in the folder. 

Example: [Example 1](https://github.com/njzjz/kconmd/blob/master/examples/example.py) [Example 2](https://github.com/njzjz/AIMDFragmentation/blob/master/examples/example.py)
