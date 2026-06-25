<div align="center">
<h1>🌬️ Distributed Smart Fan Controller</h1>

<p>
<a href="#"><img src="[https://img.shields.io/badge/STM32-%2303234B.svg?style=for-the-badge&logo=stmicroelectronics&logoColor=white](https://www.google.com/search?q=https://img.shields.io/badge/STM32-%252303234B.svg%3Fstyle%3Dfor-the-badge%26logo%3Dstmicroelectronics%26logoColor%3Dwhite)" alt="STM32"></a>
<a href="#"><img src="[https://img.shields.io/badge/Raspberry%20Pi%20Pico-C51A4A?style=for-the-badge&logo=Raspberry%20Pi&logoColor=white](https://www.google.com/search?q=https://img.shields.io/badge/Raspberry%2520Pi%2520Pico-C51A4A%3Fstyle%3Dfor-the-badge%26logo%3DRaspberry%2520Pi%26logoColor%3Dwhite)" alt="Raspberry Pi Pico"></a>
<a href="#"><img src="[https://img.shields.io/badge/FreeRTOS-%2320232a.svg?style=for-the-badge&logo=freertos&logoColor=white](https://www.google.com/search?q=https://img.shields.io/badge/FreeRTOS-%252320232a.svg%3Fstyle%3Dfor-the-badge%26logo%3Dfreertos%26logoColor%3Dwhite)" alt="FreeRTOS"></a>
<a href="#"><img src="[https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white](https://www.google.com/search?q=https://img.shields.io/badge/C-00599C%3Fstyle%3Dfor-the-badge%26logo%3Dc%26logoColor%3Dwhite)" alt="C"></a>
<a href="#"><img src="[https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white](https://www.google.com/search?q=https://img.shields.io/badge/CMake-%2523008FBA.svg%3Fstyle%3Dfor-the-badge%26logo%3Dcmake%26logoColor%3Dwhite)" alt="CMake"></a>
</p>

<p><b>A multi-node, RTOS-driven embedded system designed to mirror modern automotive Electronic Control Unit (ECU) architectures.</b></p>
</div>

<br />

<h2>📑 Table of Contents</h2>
<ul>
<li><a href="#-system-architecture">System Architecture</a></li>
<li><a href="#-fault-tolerance--hardening">Fault Tolerance &amp; Hardening</a></li>
<li><a href="#-hardware-setup">Hardware Setup</a></li>
<li><a href="#-repository-structure">Repository Structure</a></li>
<li><a href="#-getting-started">Getting Started</a></li>
<li><a href="#-development-roadmap">Development Roadmap</a></li>
</ul>

<hr />

<h2 id="-system-architecture">🧠 System Architecture</h2>
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

<h3>Node 2: Motor Actuator (Raspberry Pi Pico)</h3>
<ul>
<li><strong>Environment:</strong> Bare-metal C (RP2040 SDK)</li>
<li><strong>Responsibilities:</strong>
<ul>
<li><strong>Bus Listener:</strong> Parses incoming UART frames from the STM32.</li>
<li><strong>Actuation:</strong> Maps commanded percentage (0-100%) to a hardware PWM duty cycle to drive a DC motor.</li>
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

<h2 id="-fault-tolerance--hardening">🛡️ Fault Tolerance &amp; Hardening (Phase 4)</h2>
<p>This project goes beyond simply spinning a motor by implementing production-grade edge case handling:</p>
<ul>
<li><strong>Communication Timeouts:</strong> The STM32 actively monitors the bus. If the physical wire is unplugged, the system safely transitions to an error state.</li>
<li><strong>EMI / Noise Rejection:</strong> The bare-metal Pico parser validates a checksum on every incoming packet. If electrical noise from the DC motor corrupts a byte, the packet is safely discarded to prevent erratic motor behavior.</li>
</ul>

<hr />

<h2 id="-hardware-setup">🔌 Hardware Setup</h2>
<p>To establish the physical communication link between the two nodes, wire them as follows:</p>
<table>
<thead>
<tr>
<th>STM32 (Node 1)</th>
<th>Raspberry Pi Pico (Node 2)</th>
<th>Purpose</th>
</tr>
</thead>
<tbody>
<tr>
<td><code>UART TX</code></td>
<td><code>UART RX</code></td>
<td>Transmit commands to Pico</td>
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

<h2 id="-repository-structure">📂 Repository Structure</h2>
<pre><code>smart-fan-controller/
├── shared/                 # Shared protocol header (Single Source of Truth)
│   └── comms_protocol.h
├── node1_stm32_main/       # STM32CubeIDE Project (FreeRTOS)
└── node2_pico_motor/       # CMake Project (Bare-metal RP2040)</code></pre>

<hr />

<h2 id="-getting-started">🚀 Getting Started</h2>

<h3>Prerequisites</h3>
<ul>
<li><a href="[https://www.st.com/en/development-tools/stm32cubeide.html](https://www.st.com/en/development-tools/stm32cubeide.html)">STM32CubeIDE</a> (For Node 1)</li>
<li><a href="[https://github.com/raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)">Raspberry Pi Pico SDK &amp; CMake</a> (For Node 2)</li>
<li>ARM GNU Toolchain (<code>arm-none-eabi-gcc</code>)</li>
</ul>

<h3>Building Node 1 (STM32)</h3>
<ol>
<li>Open <strong>STM32CubeIDE</strong>.</li>
<li>Select <code>File</code> &rarr; <code>Open Projects from File System</code>.</li>
<li>Point the directory to <code>/node1_stm32_main/</code>.</li>
<li>Build the project and flash using an ST-Link.</li>
</ol>

<h3>Building Node 2 (Pico)</h3>
<p>Open your terminal and run the following commands:</p>
<pre><code>cd node2_pico_motor
mkdir build &amp;&amp; cd build
cmake ..
make</code></pre>
<p>Flash the resulting <code>.uf2</code> file to the Raspberry Pi Pico by holding the <strong>BOOTSEL</strong> button while plugging it in via USB, then drag and drop the file.</p>

<hr />

<h2 id="-development-roadmap">📅 Development Roadmap (7-Day Sprint)</h2>
<ul style="list-style-type: none; padding-left: 0;">
<li><input type="checkbox" checked disabled> <strong>Day 1:</strong> Project Scaffolding &amp; Toolchain Verification</li>
<li><input type="checkbox" disabled> <strong>Day 2:</strong> Phase 1 - Foundation (Hardware &amp; Custom Protocol)</li>
<li><input type="checkbox" disabled> <strong>Day 3:</strong> Phase 2 - Pico Motor Node (Hardware PWM &amp; Parser)</li>
<li><input type="checkbox" disabled> <strong>Day 4:</strong> Phase 3A - STM32 RTOS Foundation &amp; Button Tasks</li>
<li><input type="checkbox" disabled> <strong>Day 5:</strong> Phase 3B - FSM State Manager &amp; UART TX Task</li>
<li><input type="checkbox" disabled> <strong>Day 6:</strong> Phase 4 - Hardening (Timeouts &amp; Checksum Validation)</li>
<li><input type="checkbox" disabled> <strong>Day 7:</strong> Integration, Tuning &amp; Documentation</li>
</ul>
