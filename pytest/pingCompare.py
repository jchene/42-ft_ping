import subprocess
import json
import difflib

def run_ping(command):
    """Exécute une commande ping et retourne la sortie."""
    try:
        result = subprocess.run(command, shell=True, text=True, capture_output=True)
        return result.stdout.strip(), result.stderr.strip()
    except Exception as e:
        return f"Erreur lors de l'exécution: {e}", ""

def compare_outputs(output1, output2):
    """Compare deux sorties et affiche les différences."""
    diff = list(difflib.unified_diff(output1.splitlines(), output2.splitlines(), lineterm=""))
    return "\n".join(diff) if diff else "Aucune différence."

def main():
    with open("args.json", "r") as file:
        data = json.load(file)
    
    for args in data["args"]:
        print(f"\n\033[32m=== Test avec arguments: {args} ===\033[0m")
        sys_command = f"ping {args}"
        my_command = f"../bin/ft_ping {args}"
        
        sys_out, sys_err = run_ping(sys_command)
        my_out, my_err = run_ping(my_command)
        
        print("\033[31mDifférences STDOUT:\033[0m")
        print(compare_outputs(sys_out, my_out))
        
        print("\033[33mDifférences STDERR:\033[0m")
        print(compare_outputs(sys_err, my_err))

if __name__ == "__main__":
    main()
