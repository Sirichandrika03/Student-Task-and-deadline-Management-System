-- ========================================================
-- STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM
-- Advanced Data Structures (ADS) - PBL Project
-- Database Schema: student_task_db
-- ========================================================

CREATE DATABASE IF NOT EXISTS student_task_db
CHARACTER SET utf8mb4
COLLATE utf8mb4_unicode_ci;

USE student_task_db;

-- --------------------------------------------------------
-- 1. Table structure for table `Students`
-- --------------------------------------------------------
DROP TABLE IF EXISTS `Tasks`;
DROP TABLE IF EXISTS `Subjects`;
DROP TABLE IF EXISTS `Students`;

CREATE TABLE `Students` (
    `student_id` INT AUTO_INCREMENT PRIMARY KEY,
    `full_name` VARCHAR(100) NOT NULL,
    `email` VARCHAR(100) NOT NULL UNIQUE,
    `password_hash` VARCHAR(255) NOT NULL,
    `roll_number` VARCHAR(50) NOT NULL UNIQUE,
    `department` VARCHAR(100) DEFAULT 'Computer Science & Engineering',
    `semester` INT DEFAULT 5,
    `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------
-- 2. Table structure for table `Subjects`
-- --------------------------------------------------------
CREATE TABLE `Subjects` (
    `subject_id` INT AUTO_INCREMENT PRIMARY KEY,
    `student_id` INT NOT NULL,
    `subject_code` VARCHAR(20) NOT NULL,
    `subject_name` VARCHAR(100) NOT NULL,
    `semester` INT DEFAULT 5,
    `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT `fk_subjects_student`
        FOREIGN KEY (`student_id`) REFERENCES `Students` (`student_id`)
        ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------
-- 3. Table structure for table `Tasks`
-- --------------------------------------------------------
CREATE TABLE `Tasks` (
    `task_id` INT AUTO_INCREMENT PRIMARY KEY,
    `student_id` INT NOT NULL,
    `subject_id` INT NULL,
    `title` VARCHAR(255) NOT NULL,
    `description` TEXT,
    `priority` ENUM('High', 'Medium', 'Low') NOT NULL DEFAULT 'Medium',
    `deadline` DATETIME NOT NULL,
    `status` ENUM('Pending', 'Completed') NOT NULL DEFAULT 'Pending',
    `completed_at` DATETIME NULL,
    `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX `idx_task_student` (`student_id`),
    INDEX `idx_task_deadline` (`deadline`),
    INDEX `idx_task_priority` (`priority`),
    INDEX `idx_task_status` (`status`),
    CONSTRAINT `fk_tasks_student`
        FOREIGN KEY (`student_id`) REFERENCES `Students` (`student_id`)
        ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT `fk_tasks_subject`
        FOREIGN KEY (`subject_id`) REFERENCES `Subjects` (`subject_id`)
        ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ========================================================
-- Sample Seed Data
-- ========================================================

-- Insert Demo Student (Password is 'student123' hashed using Werkzeug pbkdf2)
-- pbkdf2:sha256:600000$g7bZ... or werkzeug scrypt/pbkdf2
INSERT INTO `Students` (`student_id`, `full_name`, `email`, `password_hash`, `roll_number`, `department`, `semester`) VALUES
(1, 'Alex Rivers', 'student@college.edu', 'scrypt:32768:8:1$uD6R2rM9y1l0dF3A$77c385f95886fe8e5e7836154133496e5eb6c429bbcd18ba7ee92a7f05351a44efce52243e8d2e8250ea54e38e8be34f590ff9aa95f9e2b175adabff6b2511fe', '23CS101', 'Computer Science & Engineering', 5),
(2, 'Siri Chandrika', 'siri@college.edu', 'scrypt:32768:8:1$uD6R2rM9y1l0dF3A$77c385f95886fe8e5e7836154133496e5eb6c429bbcd18ba7ee92a7f05351a44efce52243e8d2e8250ea54e38e8be34f590ff9aa95f9e2b175adabff6b2511fe', '23CS102', 'Computer Science & Engineering', 5);

-- Insert Subjects for Student 1
INSERT INTO `Subjects` (`subject_id`, `student_id`, `subject_code`, `subject_name`, `semester`) VALUES
(1, 1, 'CS501', 'Advanced Data Structures & Algorithms', 5),
(2, 1, 'CS502', 'Database Management Systems', 5),
(3, 1, 'CS503', 'Operating Systems', 5),
(4, 1, 'CS504', 'Computer Networks', 5),
(5, 1, 'CS505', 'Software Engineering & Agile', 5),
(6, 2, 'CS501', 'Advanced Data Structures & Algorithms', 5);

-- Insert Tasks for Student 1 demonstrating various priorities and deadlines
INSERT INTO `Tasks` (`task_id`, `student_id`, `subject_id`, `title`, `description`, `priority`, `deadline`, `status`, `completed_at`) VALUES
(101, 1, 1, 'Implement Red-Black Tree in C', 'Write complete insertion, deletion, and rotation logic with self-balancing verification.', 'High', DATE_ADD(NOW(), INTERVAL 1 DAY), 'Pending', NULL),
(102, 1, 1, 'Merge Sort & Quick Sort Benchmark', 'Compare runtime complexity O(n log n) across 100,000 randomized integers.', 'Medium', DATE_ADD(NOW(), INTERVAL 3 DAY), 'Pending', NULL),
(103, 1, 2, 'Design Normalized ER Diagram', 'Deliver 3NF schema for campus library with SQL constraints and triggers.', 'High', DATE_ADD(NOW(), INTERVAL 2 DAY), 'Pending', NULL),
(104, 1, 3, 'Dining Philosophers Problem Simulation', 'Solve semaphore deadlocks and starvation in POSIX pthread environment.', 'Medium', DATE_ADD(NOW(), INTERVAL 5 DAY), 'Pending', NULL),
(105, 1, 4, 'TCP vs UDP Socket Programming Lab', 'Build multithreaded chat client and packet loss measurement utility.', 'Low', DATE_ADD(NOW(), INTERVAL 7 DAY), 'Pending', NULL),
(106, 1, 5, 'Sprint Retrospective & Jira Board Review', 'Compile sprint burndown chart and document user story velocity metrics.', 'Low', DATE_ADD(NOW(), INTERVAL 10 DAY), 'Pending', NULL),
(107, 1, 1, 'Binary Search on Sorted Task IDs', 'ADS Assignment 1: Implement iterative and recursive binary search.', 'High', DATE_SUB(NOW(), INTERVAL 2 DAY), 'Completed', DATE_SUB(NOW(), INTERVAL 2 DAY)),
(108, 1, 2, 'SQL Complex Join Queries Lab', 'ADS Assignment 2: Practice nested subqueries, grouping, and window functions.', 'Medium', DATE_SUB(NOW(), INTERVAL 1 DAY), 'Completed', DATE_SUB(NOW(), INTERVAL 1 DAY));

