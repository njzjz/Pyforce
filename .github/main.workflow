workflow "Compile" {
  on = "push"
  resolves = ["docker://gcc:9"]
}

action "docker://gcc:9" {
  uses = "docker://gcc:9@sha256:125a7b7c73b13677a9c4affc8a5a8c0aae172b20bab56f3fc01266b6dac8966a"
  runs = "bash"
  args = ["-c", "git clone https://github.com/lammps/lammps lammps -b stable_12Dec2018 --depth=1 && cp -r USER-PYFORCE/ lammps/src && cd lammps/src && make yes-user-pyforce && make serial"]
}
