from colorama import Fore
from colorama import init as colorama_init

def print_sucess(message):
    colorama_init(autoreset=True)
    print(Fore.GREEN + message + Fore.RESET)

def print_failure(message):
    colorama_init(autoreset=True)
    print(Fore.RED + message + Fore.RESET)