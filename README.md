# Introduction to Robotics (2023 - 2024)

This repository contains the laboratory homeworks for the "Introduction to Robotics" course, taken in the 3rd year at the Faculty of Mathematics and Computer Science, University of Bucharest. Each homework includes requirements, implementation details, code and image files.

<details>
 	<summary>Homework #1 - RGB LED controlled by 3 potentiometers</summary>
	<h2>Task Requirements</h2>
	Use a separate potentiometer for controlling each color of the RGB LED: Red, Green, and Blue. This control must leverage digital electronics. Specifically, you need to read the potentiometer’s value with Arduino and then write a mapped value to the LED pins.
	<h2>Photo of the setup</h2>
	<img src="Assets/rgb.jpg">
	<h2>Video</h2>
	<a href="https://youtu.be/r5gD0jrxwg8">YouTube link</a>
	
</details>

<details>
	<summary>Homework #2 - Elevator simulator wannabe</summary>
	<h2>Task Requirements</h2>
	Design a control system that simulates a 3-floor elevator using the Arduino platform. Here are the specific requirements:
	<ul>
		<li>LED Indicators: Each of the 3 LEDs should represent one of the 3 floors. The LED corresponding to the current floor should light up. Additionally, another LED should represent the elevator’s operational state. It should blink when the elevator is moving and remain static when stationary.</li>
		<li>Buttons: Implement 3 buttons that represent the call buttons from the 3 floors. When pressed, the elevator should simulate movement towards the floor after a short interval (2-3 seconds).
		</li>
		<li>Buzzer (optional for Computer Science, mandatory for CTI): The buzzer should sound briefly during the following scenarios:
			<ul>
				<li>Elevator arriving at the desired floor (something resembling a ”cling”).</li>
				<li>Elevator doors closing and movement (pro tip: split them into 2 different sounds).</li>
			</ul>
		</li>
		<li>State Change & Timers: If the elevator is already at the desired floor, pressing the button for that floor should have no effect. Otherwise, after a button press, the elevator should ”wait for the doors to close” and then ”move” to the corresponding floor. If the elevator is in movement, it should either do nothing or it should stack its decision (get to the first programmed floor, open the doors, wait, close them and then go to the next desired floor).</li>
		<li>Debounce: Remember to implement debounce for the buttons to avoid unintentional repeated button presses.</li>
	</ul>
	<h2>Photo of the setup</h2>
	<img src="Assets/homework_2.jpg">
	<h2>Video</h2>
	<a href="https://youtu.be/LfgWHZSbjWU">YouTube link</a>

</details>

<details>
	<summary>Homework #3 - 7 segment display drawing</summary>
	<h2>Task Requirements</h2>
	Use the joystick to control the position of the segment and ”draw” on the display. The movement between segments should be natural, meaning they should jump from the current position only to neighbors, but without passing through ”walls”. Requirements:
	<ul>
		<li>The initial position should be on the DP. The current position always blinks (irrespective of the fact that the segment is on or off). Use the joystick to move from one position to neighbors. Short pressing the button toggles the segment state from ON to OFF or from OFF to ON. Long pressing the button resets the entire display by turning all the segments OFF and moving the current position to the decimal point.</li>
	</ul>	
	<h2>Photo of the setup</h2>
	<img src="Assets/homework_3.jpg">
	<h2>Video</h2>
	<a href="https://youtu.be/cHurHx7VuAc">YouTube link</a>
	</ul>

</details>

<details>
	<summary>Homework #4 - Stopwatch Timer</summary>
	<h2>Task Requirements</h2>
	<p>Use a 4 digit 7 segment display and 3 buttons to implement a stopwatch timer that counts in 10ths of a second and has a save lap functionality (similar to most basic stopwatch functions on most phones).</p>
	<p>The starting value of the 4 digit 7 segment display should be ”000.0”. Your buttons should have the following functionalities:</p>
	<ul>
		<li>Button 1: Start / pause.</li>
		<li>Button 2: Reset (if in pause mode). Reset saved laps (if in lap viewing mode).</li>
		<li>Button 3: Save lap (if in counting mode), cycle through last saved laps (up to 4 laps).</li>
	</ul>	
	<h2>Photo of the setup</h2>
	<img src="Assets/homework_4.jpg">
	<h2>Video</h2>
	<a href="https://www.youtube.com/watch?v=WpZOTJZQCmM">YouTube link</a>
	</ul>
</details>