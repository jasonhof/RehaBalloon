# RehaBalloon

"Rehab Balloon" is an inexpensive sensor-actuator combination that is envisioned to help users accomplish and track specific types of physical rehabilitation.

The Arduino with integrated pressure sensor is inserted into a balloon filled up with air.  If the user is required to hold a specific pressure to improve their fine motor skills, the balloon will provide light-up feedback to indicate when the correct pressure is held.  If the user is required to pump their arms together or squeeze their hand, a TFLite ML model will classify the pumps as "quick" or "normal" and record the total number of exercises in that session.

More background research and finalization of the project needs to be done, but we prototyped the device in about 24 hours as part of the "Hacking with Hardware" on the campus of LMU Munich.

![image](https://user-images.githubusercontent.com/5325477/236141831-cf9bed38-ad3f-4be4-8966-4a861e5218b8.png)

## Tutorials Used:
- [XIAO BLE](https://wiki.seeedstudio.com/XIAO_BLE/)
- [TFLite on XIAO nRF52840](https://wiki.seeedstudio.com/XIAO-BLE-Sense-TFLite-Getting-Started/)

## Acknowledgements

I worked with Pallabi and Giovanni on this project.  Special thanks to the Media Informatics group at LMU Munich, especially Dr. Sven Mayer, Dr. Sebastian Feger, Sophia Sakel, and Jesse Grootjen for organizing the workshop and sharing prototype hardware and starter code with us.
