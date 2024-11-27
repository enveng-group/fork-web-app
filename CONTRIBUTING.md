# Contributing to Web-App Repository

Thank you for considering contributing to Web-App! We welcome contributions from everyone. By participating in this project, you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT).

## How to Contribute

### Reporting Issues

If you encounter any issues or have questions, please check the [GitHub Issues](https://github.com/enssol/web-app/issues) page. You can search for existing issues or open a new one if your issue has not been addressed. When opening a new issue, please include the following information:

- A clear and descriptive title
- A detailed description of the issue
- Steps to reproduce the issue
- Any relevant logs, screenshots, or other information

### Submitting Pull Requests

We welcome pull requests for bug fixes, improvements, and new features. To submit a pull request, follow these steps:

1. **Fork the repository**: Click the "Fork" button at the top right corner of the repository page.

2. **Clone your fork**: Clone your forked repository to your local machine.

   ```sh
   git clone git@github.com:your-username/web-app.git
   cd web-app
   ```

### What Not to Commit

Please avoid committing the following types of files to the repository:

- **Generated assets**: Object files, compiled binaries, and other generated files should not be committed. These files waste space and can lead to stale data.
- **Configuration files with sensitive information**: Files containing keys, passwords, environment variables, or machine-specific configurations should not be committed. Remove any references to file paths or settings specific to your machine.
- **Binary dependencies**: Dependencies that you do not control, such as external libraries or frameworks, should not be committed. Use a package manager to handle these dependencies whenever possible.

By following these guidelines, you help keep the repository clean and maintainable for everyone\.
