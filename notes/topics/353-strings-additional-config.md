---
tags: [config, strings, legal, apptracer, contact-provider, surveillance]
status: confirmed
sources:
  - work/apktool_base/res/values/strings.xml
related:
  - "[[352-apptracer-app-token]]"
  - "[[351-firebase-config-keys]]"
---

# strings.xml — дополнительные конфигурационные значения

Дополнение к [[351-firebase-config-keys]] и [[352-apptracer-app-token]].

## Apptracer

| Ключ | Значение |
|---|---|
| `tracer_app_token` | `t6QnlHov0Gq1UBGYG9GPqZu0EiVMZ922FKvwyAEASa90` |
| `tracer_mapping_uuid` | `47afde00-4ab5-11f1-9a3d-0c152d90928f` |

**`tracer_mapping_uuid`** — UUID маппинга символов для Apptracer. Используется для декодирования обфусцированных стек-трейсов.

## Юридические ссылки

| Ключ | Значение |
|---|---|
| `oneme_privacy_policy_link` | `https://legal.max.ru/pp` |
| `oneme_user_agreement_link` | `https://legal.max.ru/ps` |
| `oneme_faq_restricted_join_link` | `https://help.max.ru/help/about/kak-zaregistrirovatsya-v-max` |

## Contact Provider

| Ключ | Значение |
|---|---|
| `tt_contact_mimetype` | `vnd.android.cursor.item/vnd.ru.ok.tamtam.android.profile` |

**`tt_contact_mimetype`** — MIME-тип для контактов MAX в адресной книге Android. Позволяет другим приложениям идентифицировать контакты MAX.

## Сводка

`tracer_mapping_uuid=47afde00-4ab5-11f1-9a3d-0c152d90928f`. Юридические ссылки: `legal.max.ru/pp`/`legal.max.ru/ps`. Contact MIME: `vnd.android.cursor.item/vnd.ru.ok.tamtam.android.profile`.
