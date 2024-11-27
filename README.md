# Web-App

##

### Project overview

The **WebApp** is designed to provide an efficient, scalable, and maintainable web application tailored to environmental engineering needs. The key goals of this project include:

- **Building a scalable, maintainable codebase.**
- **Ensuring clarity through Data-Oriented Programming (DOP).**
- **Maintaining an organised and efficient development process using 5S methodology.**

#### Key Objectives

- Implement user authentication and core features.
- Optimise data handling and performance.
- Maintain clarity and organisation throughout the project lifecycle.

---

### Table of Contents

1. [[Project Overview](#project-overview)](#Project-overview)
2. [[Features](#features)](#Features)
3. [[Methodologies](#methodologies)](#Methodologies)
    - [[5S Implementation](#5s-implementation)](#5s-implementation)
    - [[Data-Oriented Programming](#data-oriented-programming)](#data-oriented-programming)
4. [[Set-Up Instructions](#set-up-instructions)](#set-up-instructions)
5. [[Git Workflow](#git-workflow)](#git-workflow)
    - [[Branching Strategy](#branching-strategy)](#branching-strategy)
    - [[Commit Guidelines](#commit-guidelines)](#commit-guidelines)
6. [[CI/CD Workflow](#cicd-workflow)](#cicd-workflow)
7. [[Contributing](#contributing)](#contributing)
8. [[Licence](#licence)](#licence)
9. [[Contact](#contact)](#contact)

---

### Features

- **User Authentication and Authorisation**: Secure login and user management system.
- **Dashboard**: Manage and visualise environmental data.
- **API Integrations**: Fetch real-time updates from third-party sources.
- **Performance Optimised**: Clean, maintainable, and efficient codebase ensuring smooth operation.

---

### Methodologies

Our team uses the **5S methodology** to keep the project organised and efficient:

- **Sort**: Remove unnecessary tasks, features, and code.
- **Set in Order**: Organise files, code, and tasks for easy access.
- **Shine**: Keep the code and project environment clean with regular reviews and automated checks.
- **Standardise**: Establish coding standards, guidelines, and reusable templates.
- **Sustain**: Encourage continuous improvement and regular audits to ensure long-term project health.

---

#### 5S Implementation

##### Sort

- Objective: Identify and eliminate unnecessary items from the project workspace.
- Actions:
Conduct an initial audit of all project resources (code, documents, tools).
Remove or archive obsolete or redundant items.

##### Set in Order

- Objective: Organize remaining items for easy access and efficiency.
- Actions:
Establish a logical structure for code repositories.
Create a standardized directory and file naming convention.
Implement version control practices.

##### Shine

- Objective: Maintain a clean and organized workspace.
- Actions:
Schedule regular code reviews and refactoring sessions.
Implement automated tools for code quality checks.

##### Standardize

- Objective: Establish standards for organization and cleanliness.
- Actions:
Develop coding standards and best practices documentation.
Create templates for common tasks and documentation.

##### Sustain

- Objective: Maintain and review standards consistently.
- Actions:
Conduct regular training sessions on 5S practices.
Schedule periodic audits to ensure compliance with standards.

#### Data-Oriented Programming

The **Data-Oriented Programming (DOP)** methodology is employed to enhance performance, maintainability, and scalability:

- **Data Separation**: Separate data from application logic to maintain clean, reusable code.
- **Efficient Data Handling**: Use optimised data structures that ensure high performance.
- **Immutability**: Follow immutable data practices to ensure consistency across the application.

---

### Set-Up Instructions

Follow these steps to get the development environment up and running:

1. **Clone the repository**

   ```sh
   git clone <https://github.com/your-org/web-app.git>
   cd web-app
   ```

### Git Workflow

To ensure collaboration and maintain project quality, we follow a structured Git workflow:

---

#### Branching Strategy

- Main: The stable, production-ready branch.
- Develop: The integration branch for new features and fixes.
- Feature Branches: Use feature/feature-name for new features.
- Bugfix Branches: Use bugfix/bug-description for bug fixes.

### Commit Guidelines

- Write atomic commits focused on a single change or fix.
- Use descriptive commit messages to ensure clarity.
    - Example:

        - git commit -m "Add user authentication endpoint"
        - CI/CD Workflow

We use Continuous Integration (CI) and Continuous Delivery (CD) to automate testing and deployment.

---

### CI/CD Workflow

#### Continuous Integration (CI)

- Automated builds and tests run for every push to the develop branch.
Static code analysis is used to maintain code quality.

#### Continuous Delivery (CD)

- Deployments to a staging environment are automated for integration testing.
Production deployment requires manual approval.
- Pipeline Example
    - Refer to the .github/workflows/ci-cd.yml file for detailed CI/CD pipeline configuration.

---

### Contributing

We welcome contributions to improve the WebApp. If you'd like to contribute, follow these steps:

Fork the repository and clone it to your local machine.
Create a new feature or bugfix branch.
Make your changes, ensuring your commits follow the commit guidelines.
Submit a pull request to the develop branch for review.
For more information, refer to our [CONTRIBUTING.md](CONTRIBUTING.md) file.

---

### Licence

This project is licensed under the [LICENSE](LICENSE). Please review it for details on usage and distribution.

---

### Contact

For any inquiries or questions regarding this project, feel free to reach out to the team:

Lead Developer: [Adrian Gallo](agallo@enveng-group.com.au)

Project Maintainer: [Adrian Gallo](agallo@enveng-group.com.au)
