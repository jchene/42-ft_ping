#!/usr/bin/env python3
import json
import subprocess

def run_command(command, expected_err):
	# Lance la commande en ignorant la sortie standard et en capturant uniquement la sortie d'erreur
	proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
	stdout, stderr = proc.communicate()

	# Compare le contenu d'erreur avec ce qui est attendu
	if stderr.strip() == expected_err.strip():
		print(f"\033[32m[OK] Commande: {command}\033[0m")
		print(stdout, end="")
	else:
		print(f"\033[31m[FAIL] Commande: {command}\033[0m")
		print(stdout, end="")
		print("Erreur obtenue:")
		print(stderr)
		print("Erreur attendue:")
		print(expected_err)
	
def main():
	# Lecture du fichier de configuration
	try:
		filename = sys.argv[1] if len(sys.argv) > 1 else "config.json"
		with open(filename, "r") as f:
    		config = json.load(f)
	except Exception as err:
		print("Erreur lors de la lecture du fichier de configuration:", err)
		return

	tests = config.get("tests", [])
	if not tests:
		print("Aucun test trouvé dans le fichier de configuration.")
		return

	# Exécute chaque test
	for test in tests:
		comment = test.get("_comment")
		if comment:
			print("\033[30m"+comment+":\033[0m")
		commands = test.get("commands")
		expected_output = test.get("expected_output")
		for command in commands:
			run_command(command, expected_output)

if __name__ == "__main__":
	main()
