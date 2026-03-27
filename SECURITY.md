# Security Policy / セキュリティポリシー

## Supported Versions / サポート対象

This policy applies only to the `main` branch of the Siv3D v0.8 development repository (`siv8`).

このポリシーは、Siv3D v0.8 開発用リポジトリ（`siv8`）の `main` ブランチのみに適用されます。

## Reporting a Vulnerability / 脆弱性の報告方法

Please use **[GitHub Private Vulnerability Reporting](https://github.com/Siv3D/siv8/security/advisories/new)** to report security issues in this repository.

このリポジトリに関するセキュリティ上の問題は、**[GitHub Private Vulnerability Reporting](https://github.com/Siv3D/siv8/security/advisories/new)** を使って報告してください。

Reports are reviewed in light of severity, reproducibility, and ongoing development priorities. We aim to send an initial response within 7 days, but the timing of further investigation, mitigation, or a fix may vary depending on the issue and project priorities.

報告は、重大度、再現性、開発状況を踏まえて確認されます。初回返信は 7 日以内を目標としますが、その後の調査・緩和策・修正の時期は、問題の内容やプロジェクトの優先度に応じて変動します。

## Scope / 対象範囲

We welcome reports of vulnerabilities that may affect the security of systems using siv8, including issues such as memory corruption, sandbox escapes, unintended file access, code execution, or other vulnerabilities with a plausible security impact.

siv8 を利用するシステムの安全性に影響し得る脆弱性の報告を歓迎します。たとえば、メモリ破壊、サンドボックス逸脱、意図しないファイルアクセス、任意コード実行、その他セキュリティ上の影響が合理的に想定できる問題が含まれます。

Reports based only on automated scanner output, version strings, or known CVEs in dependencies are generally out of scope unless they demonstrate a concrete security impact in siv8 itself.

自動スキャナの出力、バージョン文字列、依存関係の既知 CVE の列挙だけに基づく報告は、siv8 自体における具体的なセキュリティ影響が示されない限り、原則として対象外です。

If a dependency issue can be shown to have a real security impact through siv8, even a simple reproduction or concise explanation is sufficient for the report to be considered in scope.

依存関係の問題であっても、siv8 を通じて実際のセキュリティ影響が生じることが示される場合は、簡単な再現手順や簡潔な説明だけでも対象として扱います。

We may ask follow-up questions or request a minimal reproduction to validate the reported impact.

影響確認のため、追加情報や最小再現コードの提供をお願いする場合があります。

## Disclosure / 公開について

Please avoid public disclosure before a fix or mitigation is available, where possible. When appropriate, maintainers may coordinate disclosure through release notes, a GitHub Security Advisory, or other suitable channels.

可能であれば、修正または緩和策が利用可能になる前の公開は控えてください。必要に応じて、公開はリリースノート、GitHub Security Advisory、その他適切な手段を通じてメンテナが調整する場合があります。
