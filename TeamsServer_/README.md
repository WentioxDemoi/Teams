# Teams Server

Backend server for the Teams application, handling authentication, messaging, and real-time communication.

## Project Architecture

```text
src/
├── Core/            # Core business logic and models
│   ├── Models/      # Data structures (e.g. User)
│   ├── Registeries/ # User session pool
│   ├── Repositories/# Data access (UserRepository, etc.)
│   └── Services/    # Business logic (AuthService, etc.)
├── Handlers/        # Incoming request handling and routing
├── Infrastructure/  # Technical infrastructure (DB, ConnectionPool, QueryBuilder, etc.)
├── Network/         # TCP/SSL servers and network sessions (TcpListener, AuthSession)
├── Utils/           # General utilities (Crypto, Config, ResponseFormater, BoostErrorHandler)
└── main.cpp         # Application entry point

This project uses a layered architecture with dependency injection.
```

## Prerequisites

- Docker >= 28.4.0
- CMake >= 3.22.1
- Boost >= 1.74.0
- SQLite >= 3.54.0
- GoogleTest >= 1.17.0
- g++ >= 11.4.0

## Deployment with Docker

The server is built and run using Docker, ensuring a consistent environment with the EC2 deployment environment.

### Method 1: Docker Compose

1. Start the server and database:
```bash
docker-compose --env-file .env up --build
```

2. To reset the environment:
```bash
docker-compose down -v
```

### Method 2: DevContainer (Recommended)

1. Launch the DevContainer through VSCode (`CMD + SHIFT + P`) → **Rebuild Container**.

2. Once inside the container, build and run the project manually.

## Unit Tests

Unit tests use **GoogleTest**, which is automatically integrated by CMake through FetchContent.

### Building and Running the Tests

1. Uncomment the test-related section in `CMakeLists.txt`.
2. In the Dockerfile, comment out the line that starts the server and uncomment the line that starts the terminal.
3. Once connected to the DevContainer terminal, build the project and run the tests:

```bash
cd build/
./server_tests
```

### Generating Code Coverage

The project uses **gcovr** to generate an HTML report that provides a graphical representation of code coverage.

After running the tests, still from the `build/` directory, run:

```bash
gcovr \
    --root .. \
    --filter '../src/' \
    --html-details coverage.html
```

This command generates the following file:

```text
build/coverage.html
```

Open `coverage.html` in a browser to view the coverage report, including per-file coverage and covered or uncovered lines of code.

> **Note:** The tests must be run before generating the report so that the coverage data files are properly generated.

## Configuration

The server uses a `.env` file for configuration. SSL certificates (`server.crt`, `server.key`) are provided in the repository.

**Note:** Since this project is intended for educational purposes and does not contain sensitive data, the `.env` files, SSL certificates, and VSCode configurations are included in the repository.

## Ports Used

- **8080**: Authentication server
- **8082**: Messaging server
- **8083**: WebRTC exchange server
- **8084**: Contacts server

## Dependencies

- **Boost.Asio**: Asynchronous TCP servers and thread management for the worker pool
- **libpqxx / PostgreSQL**: PostgreSQL database access
- **OpenSSL**: TLS/SSL support
- **GoogleTest & GoogleMock**: Unit tests and mocks for AuthService and UserRepository
- **Argon2**: Secure password hashing
- **gcovr**: HTML code coverage report generation

## Technical Architecture

- **Connection Pool**: Optimized PostgreSQL connection management through `ConnectionPool` and `DatabaseManager`
- **Session Management**: Secure client session management through `AuthSession` with SSL and token-based authentication
- **Handler Layer**: `Handler` and `AuthHandler` for routing and processing JSON requests
- **Async I/O**: Asynchronous connection and request processing through `asio::io_context` and `thread_pool`
- **Core Layer**: Centralized business logic in `Core/Services` and data access through `Core/Repositories`
- **Utils Layer**: Generic tools for security (`Crypto`), configuration (`Config`), and JSON formatting (`ResponseFormater`)

## Important Notes

- The CMake configuration will be expanded as development progresses
- Additional features will be added progressively
- UML diagrams are available in the `diagram` branch

## TODO

- Refactor the include system to reduce the load on Clang