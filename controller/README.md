# Robot UDP Controller

Python controller that reads an Xbox controller and sends drive commands over UDP.

## Setup

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

## Run

```bash
python3 robot_controller.py --ip 192.168.1.50 --port 9000
```

## Notes

- Left stick controls `vx` (forward/back) and `vy` (strafe).
- Right stick X controls `wz` (yaw).
- The program binds the local UDP port to the same `--port` so the robot can reply with telemetry.
- Use `--left-x-axis`, `--left-y-axis`, and `--right-x-axis` if your controller maps axes differently.
