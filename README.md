
**Title:** Inter-Process Communication Using Shared Memory and Semaphores (C)

**Description:**

This project delves into the fundamentals of inter-process communication (IPC) on single-system environments,  implemented in C. It provides a comprehensive demonstration of how shared memory and semaphores can be effectively employed to achieve robust and synchronized message exchange between two distinct processes, as outlined in `process_a.c` and `process_b.c`.

**Key Features:**

* **Shared Memory Implementation:** A dedicated shared memory segment is created to facilitate the efficient transfer of messages between processes. This shared data region empowers seamless communication without the overhead associated with other IPC methods.

* **Segmentation and Reassembly:** To optimize transmission, larger messages are strategically segmented into smaller, manageable units. These segments are stored in the shared memory buffer.  The receiving process meticulously reassembles segmented messages before delivery, maintaining message integrity.

* **Semaphore-Based Synchronization:** Semaphores are employed as indispensable synchronization mechanisms. They enforce mutual exclusion on the shared memory region, preventing race conditions and ensuring that processes access the shared data in a well-defined,  coordinated manner.

* **Termination Handling:** The code gracefully handles termination signals passed between processes.  A dedicated termination flag within the shared memory ensures  a clean exit from the communication loop upon receiving the signal.

* **Comprehensive Reporting:** The project includes a reporting component. It tracks metrics such as messages sent and received, the average number of segments per message, and average wait times. This reporting provides valuable insights into communication performance.
