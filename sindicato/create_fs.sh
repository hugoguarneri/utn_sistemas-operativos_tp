#!/bin/bash

PUNTO_MONTAJE=/home/utnso/afip_fs
rm -rf $PUNTO_MONTAJE
[[ -d "$PUNTO_MONTAJE" ]] || { echo "creando $PUNTO_MONTAJE"; mkdir -p "$PUNTO_MONTAJE"; }
[[ -d "$PUNTO_MONTAJE/Metadata" ]] || { echo "creando $PUNTO_MONTAJE/Metadata"; mkdir -p "$PUNTO_MONTAJE/Metadata"; }
[[ -d "$PUNTO_MONTAJE/Blocks" ]] || { echo "creando $PUNTO_MONTAJE/Blocks"; mkdir -p "$PUNTO_MONTAJE/Blocks"; }
[[ -d "$PUNTO_MONTAJE/Files/Recetas" ]] || { echo "creando $PUNTO_MONTAJE/Files/Recetas"; mkdir -p "$PUNTO_MONTAJE/Files/Recetas"; }
[[ -d "$PUNTO_MONTAJE/Files/Restaurantes" ]] || { echo "creando $PUNTO_MONTAJE/Files/Restaurantes"; mkdir -p "$PUNTO_MONTAJE/Files/Restaurantes"; }
touch "$PUNTO_MONTAJE/Metadata/Metadata.AFIP"
echo -e "BLOCK_SIZE=32\nBLOCKS=4096\nMAGIC_NUMBER=AFIP\n" > "$PUNTO_MONTAJE/Metadata/Metadata.AFIP"

