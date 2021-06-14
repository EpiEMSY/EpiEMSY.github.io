# tom noobs blender

It's the S4 project for EPITA

Libraries that are used in the project :
gtk+-3
SDL

Manuel de compilation :
Si l'exécutable n'est pas présent ou que vous voulez réinitialiser le best
score, il faudra recompiler. Pour ce faire, il faut taper la commande 'make
clean' dans la console dans le dossier CandyCrush et ensuite taper la commande
'make'.

Manuel d'utilisation :
Tout d'abord, il faut lancer le jeu. Pour ce faire tapez la commande 'cd
CandyCrush' pour aller dans le dossier CandyCrush. Ensuite tapez la commande
'./tomnoob' pour lancer l'exécutable. Une fenêtre s'affiche, vous pouvez maintenant choisir le mode que vous voulez.

Player mode : Pour échanger 2 fruits, vous devez cliquer sur le premier et
glisser vers le fruit avec lequel vous voulez l'échanger. Si ces 2 fruits ne sont pas côte à côte, le jeu ne fera rien.
AI mode et Neural Network mode : Vous n'avez rien à faire dans ce mode, regardez juste l'intelligence artificielle jouer et tenter de battre son record à chaque fois.

Si la tête de notre cher Tom Noob apparaît c'est qu'il est impossible d'effectuer un mouvement (peu importe le mode) car il y a encore des combinaisons en cours de destruction.

Pour relancer une partie, vous devez quitter toutes les fenêtres et retaper la
commande './tomnoob' dans la console.

Libraries :
sudo apt-get install libsdl1.2-dev
sudo apt-get install libsdl-image1.2-dev
sudo apt-get install libgtk-3-dev