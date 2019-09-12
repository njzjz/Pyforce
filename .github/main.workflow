workflow "Compile" {
  on = "push"
  resolves = ["docker://gcc:9"]
}

action "docker://gcc:9" {
  uses = "docker://gcc:9@sha256:99e83c0aeb22645bc151cbb888ad2bcdd0a606f7b16ffdc8d03211228f626b51"
  runs = "bash"
  args = ["-c", "git clone https://github.com/lammps/lammps lammps -b stable_12Dec2018 --depth=1 && cp -r USER-PYFORCE/ lammps/src && cd lammps/src && make yes-user-pyforce && make serial"]
}
