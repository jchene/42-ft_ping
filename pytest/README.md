# End-to-End Output Tester for ft_ping

This is a small AI-generated end-to-end output tester for the `ft_ping` project. It is not a part of the main project.

## JSON Configuration Structure

The JSON configuration file should have the following structure:

```json
{
    "tests": [
        {
            "_comment": "TYPE A TESTS",
            "expected_output": "TYPE A OUTPUT",
            "commands": [
                "COMMAND1",
                "COMMAND2",
                "..."
            ]
        },
        {
            "_comment": "TYPE B TESTS",
            "expected_output": "TYPE B OUTPUT",
            "commands": [
                "COMMAND3",
                "COMMAND4",
                "..."
            ]
        },
        {
            "..."
        }
    ]
}
```

## Usage

1. Create a JSON configuration file with the desired tests.
2. Run the tester script with the configuration file as input.

Example:

```sh
pytest/endToEnd.py pytest/multi.json
```

The script will execute each command listed in the configuration file and compare the actual output with the expected output, providing a pass/fail result for each test.