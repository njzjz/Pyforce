workflow "Compile" {
  on = "push"
  resolves = ["docker://gcc:9"]
}

action "docker://gcc:9" {
  uses = "docker://gcc:9@sha256:6f3e7275fe080452f803d3e098c25b81b02280772763ce5d6caf1839de56ce48"
  runs = "bash"
  args = ["-c", "git clone https://github.com/lammps/lammps lammps -b stable_12Dec2018 --depth=1 && cp -r USER-PYFORCE/ lammps/src && cd lammps/src && make yes-user-pyforce && make serial"]
}
