---
tags: [neuro-avatars, ai, registration, login, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/login/neuroavatars/NeuroAvatarsScreen.java
related:
  - "[[23-camera-mic-screen-entry-points]]"
---

# NeuroAvatarsScreen — нейро-аватары при регистрации

`NeuroAvatarsScreen` — экран выбора нейро-аватара при регистрации.

## Функциональность

- Выбор аватара из галереи (`load_from_gallery_action`)
- Съёмка фото (`take_photo_action`)
- Удаление фото (`remove_photo_action`)
- Генерация нейро-аватара из фото

## Что важно

1. **Нейро-аватары** — при регистрации пользователь может загрузить фото для генерации AI-аватара.

2. Фото передаётся на сервер для обработки.

3. Экран используется как при регистрации (`RegistrationNeuroAvatarsScreen`), так и при редактировании профиля.

## Сводка

`NeuroAvatarsScreen`: выбор/съёмка фото → генерация нейро-аватара на сервере. Используется при регистрации и редактировании профиля.
