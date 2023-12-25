## Simulation Model

Simple simulation model for the ship dynamics of a catamaran with two identical drives. In addition to the initial conditions x0, y0, vx0, vy0, phi0, and omega0, friction in the x-direction, y-direction around the axis of rotation, as well as water flow and wind drift can be taken into account.

The example calculation includes the following parameters:

|Ship mass|Moment of inertia|Friction x|
|--------|--------|----|
|   m |    Js    |  kx  |
|   100 kg    |    10 kgm²    | 40 kg/s | 

Ship mass m = 100 kg
Moment of inertia 10 kgm²
Friction x: 40 kg/s
Friction y: 80 kg/s
Torsional friction: 5 Nsm
Starting point: X=1m; Y=1m
No wind drift
No water flow
Track 1:
Thrust 10N, Rudder 0, Duration 10 seconds
Track 2:
Thrust 10N, Rudder 0.2, Duration 100 seconds
Track 3:
Thrust 5N, Rudder 0, Duration 20 seconds
