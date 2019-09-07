workflow "Compile" {
  on = "push"
  resolves = ["docker://gcc:9"]
}

action "docker://gcc:9" {
  uses = "docker://gcc:9@sha256:44f9fa69fb4d974b1c62b0feef89b0b7faa749b3505e9c05d1f45f2a6655158b"
  runs = "bash"
  args = ["-c", "git clone https://github.com/lammps/lammps lammps -b stable_12Dec2018 --depth=1 && cp -r USER-PYFORCE/ lammps/src && cd lammps/src && make yes-user-pyforce && make serial"]
}
