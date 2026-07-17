<div align="left">
<h1>Distributed Smart Fan Controller</h1>

<p>
<!-- <a href="#"><img src="https://img.shields.io/badge/STM32-%2303234B.svg?style=for-the-badge&logo=stmicroelectronics&logoColor=white" alt="STM32"></a>
<a href="#"><img src="https://img.shields.io/badge/ESP32-E7352C.svg?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32"></a>
<a href="#"><img src="https://img.shields.io/badge/FreeRTOS-%2320232a.svg?style=for-the-badge&logo=freertos&logoColor=white" alt="FreeRTOS"></a>
<a href="#"><img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white" alt="C"></a>
<a href="#"><img src="https://img.shields.io/badge/ESP--IDF-E7352C.svg?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP-IDF"></a> -->
</p>

<p><b>A multi-node, Dual-RTOS distributed embedded system designed to mirror modern automotive Electronic Control Unit (ECU) architectures.</b></p>
</div>

<br />

<h2>Table of Contents</h2>
<ul>
<li><a href="#-system-architecture">System Architecture</a></li>
<li><a href="#-fault-tolerance--hardening">Fault Tolerance &amp; Hardening</a></li>
<li><a href="#-hardware-setup">Hardware Setup</a></li>
<li><a href="#-repository-structure">Repository Structure</a></li>
<li><a href="#-getting-started">Getting Started</a></li>
<li><a href="#-development-roadmap">Development Roadmap</a></li>
</ul>

<hr />

<h2 id="-system-architecture">System Architecture</h2>
<p>The system is distributed across two microcontrollers, separating high-level system logic from low-level hardware actuation:</p>

<h3>Node 1: Main Controller (STM32)</h3>
<ul>
<li><strong>Environment:</strong> FreeRTOS</li>
<li><strong>Responsibilities:</strong>
<ul>
<li><strong>Input Handling:</strong> Reads and software-debounces user push buttons.</li>
<li><strong>State Management:</strong> Drives a 4-state Finite State Machine (FSM): <code>Off</code> &rarr; <code>Low</code> &rarr; <code>High</code> &rarr; <code>Auto</code>.</li>
<li><strong>UI/Feedback:</strong> Updates status LEDs based on the current system state.</li>
<li><strong>Bus Communication:</strong> Frames and transmits control commands over the UART bus.</li>
</ul>
</li>
</ul>

<h3>Node 2: Motor Actuator (ESP32)</h3>
<ul>
<li><strong>Environment:</strong> ESP-IDF (FreeRTOS natively)</li>
<li><strong>Responsibilities:</strong>
<ul>
<li><strong>Bus Listener:</strong> Parses incoming UART frames from the STM32 via hardware UART.</li>
<li><strong>Actuation:</strong> Maps commanded percentage (0-100%) to a hardware PWM duty cycle utilizing the ESP32's dedicated PWM peripherals to drive a DC motor.</li>
</ul>
</li>
</ul>

<h3>The Link: Physical Communication Bus</h3>
<ul>
<li><strong>Hardware:</strong> UART</li>
<li><strong>Protocol:</strong> Custom framed packets consisting of <code>[Header | Payload | Checksum]</code>.</li>
<li><strong>Performance:</strong> <code>&lt; 5ms</code> round-trip latency.</li>
</ul>

<hr />

<h2 id="-fault-tolerance--hardening">Fault Tolerance &amp; Hardening (Phase 4)</h2>
<p>This project goes beyond simply spinning a motor by implementing production-grade edge case handling:</p>
<ul>
<li><strong>Communication Timeouts:</strong> The STM32 actively monitors the bus. If the physical wire is unplugged, the system safely transitions to an error state.</li>
<li><strong>EMI / Noise Rejection:</strong> The ESP32 parser validates a checksum on every incoming packet. If electrical noise from the DC motor corrupts a byte, the packet is safely discarded to prevent erratic motor behavior.</li>
</ul>

<hr />

<h2 id="-hardware-setup">Hardware Setup</h2>
<p>To establish the physical communication link between the two nodes, wire them as follows:</p>
<table>
<thead>
<tr>
<th>STM32 (Node 1)</th>
<th>ESP32 (Node 2)</th>
<th>Purpose</th>
</tr>
</thead>
<tbody>
<tr>
<td><code>UART TX</code></td>
<td><code>UART RX</code></td>
<td>Transmit commands to ESP32</td>
</tr>
<tr>
<td><code>UART RX</code></td>
<td><code>UART TX</code></td>
<td>Receive acknowledgments (optional)</td>
</tr>
<tr>
<td><code>GND</code></td>
<td><code>GND</code></td>
<td><strong>Critical:</strong> Common ground reference</td>
</tr>
</tbody>
</table>

<hr />

<h2 id="-repository-structure">Repository Structure</h2>
<pre><code>smart-fan-controller/
├── shared/                 # Shared protocol header (Single Source of Truth)
│   └── comms_protocol.h
├── node1_stm32_main/       # STM32CubeIDE Project (FreeRTOS)
└── node2_esp32_motor/      # ESP-IDF Project (FreeRTOS)</code></pre>

<hr />

<h2 id="-getting-started">Getting Started</h2>

<h3>Prerequisites</h3>
<ul>
<li><a href="https://www.st.com/en/development-tools/stm32cubeide.html">STM32CubeIDE</a> (For Node 1)</li>
<li><a href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/">ESP-IDF Installation Manager</a> (For Node 2)</li>
</ul>

<h3>Building Node 1 (STM32)</h3>
<ol>
<li>Open <strong>STM32CubeIDE</strong>.</li>
<li>Select <code>File</code> &rarr; <code>Open Projects from File System</code>.</li>
<li>Point the directory to <code>/node1_stm32_main/</code>.</li>
<li>Build the project and flash using an ST-Link.</li>
</ol>

<h3>Building Node 2 (ESP32)</h3>
<p>Open your ESP-IDF PowerShell Environment, navigate to the node directory, and run the following commands:</p>
<pre><code>cd node2_esp32_motor
idf.py set-target esp32
idf.py build
idf.py flash monitor</code></pre>
<p>The <code>flash monitor</code> command will automatically flash the firmware via USB and open a serial monitor to view the ESP32's logging output.</p>

<hr />
