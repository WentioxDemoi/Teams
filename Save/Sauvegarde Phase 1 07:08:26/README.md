## Sauvegarde des versions fonctionnelles

Des versions fonctionnelles du projet sont conservées sous forme de binaires afin d'éviter d'avoir à tout recompiler ultérieurement et de devoir résoudre à nouveau les problèmes liés aux dépendances, notamment celles de WebRTC.

Deux versions sont actuellement sauvegardées :
- **Ancienne version de WebRTC**
- **Nouvelle version de WebRTC**

Chaque version contient les artefacts nécessaires à son utilisation :

- **Linux** : binaire à exécuter dans un container Docker
- **macOS** : fichier `.dmg` à installer

L'objectif est de conserver une version directement utilisable, indépendamment de l'état futur des dépendances et de l'environnement de compilation.