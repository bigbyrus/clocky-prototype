# Clocky Prototype

An **alarm clock on wheels** controlled by an **Arduino R3** and geared DC motors.  
When the alarm time is reached, Clocky automatically begins its programmed motion pattern.

- **Motion on Alarm** – When the alarm time is reached, Clocky starts moving around the room.
- **Manual Controls** – Two pushbuttons (connected to Arduino interrupt pins):
  - Enable/disable motion pattern
  - Enable/disable alarm mechanism
- **Custom Design** – Entire structure 3D printed and modeled in **SolidWorks**.
- **Dual Motor Control** – Powered by a SparkFun Motor Driver (**Dual TB6612FNG**).

---

## Internals
<img width="676" height="510" alt="Screenshot 2025-08-09 191011" src="https://github.com/user-attachments/assets/f7c8666e-a0a2-4926-9315-be720005dac8" />

Both geared DC motors are driven by the TB6612FNG motor driver and powered via the Arduino Uno.  
Pushbuttons allow quick toggling for motion or alarm features without reprogramming.  

---

## Motion Pattern Demo
[Video 1](https://drive.google.com/file/d/1Xim2BYGENngRl8yJdxWr_KkPQyU4Gvop/view?usp=sharing)

[Video 2](https://drive.google.com/file/d/1n1k9Xu-Xdv3UUg2e-7S826QAlP71hYY-/view?usp=sharing)

[Video 3](https://drive.google.com/file/d/1A5TBGNYpLbiU4t36lmuHBT0GEfX0Foji/view?usp=sharing)


---


## Components Used
- Arduino Uno
- SparkFun TB6612FNG Dual Motor Driver
- Geared DC Motors
- Pushbuttons
- 3D printed chassis (designed in SolidWorks)
- TM1637 LED Display


