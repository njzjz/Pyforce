workflow "Compile" {
  on = "push"
  resolves = ["docker://gcc:9"]
}

action "docker://gcc:9" {
  uses = "docker://gcc:9@sha256:c229f8a8d57b0c1180b945ce3bb865a23b4debfe71581edbe1cf5d2064300ac6"
  runs = "bash"
  args = ["-c", "git clone https://github.com/lammps/lammps lammps -b stable_12Dec2018 --depth=1 && cp -r USER-PYFORCE/ lammps/src && cd lammps/src && make yes-user-pyforce && make serial"]
}
