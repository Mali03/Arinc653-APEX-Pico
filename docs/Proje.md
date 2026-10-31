RTOS üzerinde ARINC653 Prensipleri ile Aviyonik Sürücü Geliştirme



Bu proje, katılımcıların RTOS üzerinde düşük seviye donanım sürücüleri geliştirmesini ve bu sürücüleri sivil havacılıkta zorunlu olan ARINC653 bölütlenmiş mimari (partitioned architecture) prensipleriyle bütünleştirmesini hedefler. Katılımcılar RTOS temel felsefesini (zamansal ve mekansal izolasyon, deterministik zamanlama, sağlık izleme ve hata yönetimi) düşük maliyetli bir ortamda uygulamalı oalrak deneyimler.

Proje, GPIO, UART, I2C, SPI gibi klasik sürücü katmanlarından başlayıp, bu sürücüleri partition mantığıyla izole edilmiş görevler içine yerleştiren mini bir APEX (Application Executive) katmanı tasarımıyla sonuçlanır. Böylece katılımcılar hem gömülü yazılım mühendisliğinin temel becerilerini hem de aviyonik yazılım geliştirmenin (ARINC653 ve DO-178C) kavramsal çerçevesini kazanır.

Proje kapsamında katılımcılar, önce donanımsız simülasyon ortamında ARINC653 mimarisinin temel prensiplerini kavrarlar, ardından Raspberry Pi Pico (çift çekirdek) kullanarak zamansal ve mekansal bölütlenmesini donanım seviyesinde deneyimlerler. Böylece yalnızca teorik bir standart okuması değil, gerçek zamanlı kısıtlar altında çalışan hataya dayanıklı ve deterministik bir sistemin uçtan uca tasarım sürecini uygulamalı olarak hayata geçirirler.



Teknik Beceri Kazanımları:

\-RTOS temelleri: task scheduling, mutex-semaphore, queue tabanlı üretictüketici deseni

\-Düşük seviye sürücü geliştirme: register seviyesinde donanım kontrolü, interrupt handling, DMA mantığı

\-C dilinde bellek güvenli deterministik kod yazma disiplini

\-Bölütlenmiş mimari tasarımı: partition izolasyonu, zaman dilimli (time triggered) scheduler yazma

\-Hata yönetimi ve sistem sağlamlığı: Healt Monitor benzeri hata tespit-kurtarma mekanizmaları tasarlama



Kavramsal Kazanımlar:

\-ARINC653 standardının temel yapı taşlarına hakimiyet (partition, process, port, major-minor frame, APEX API)

\-Aviyonik yazılım geliştirmede güvenlik kritik sistem tasarımı zihniyeti (DO-178C giriş niteliğinde farkındalık)

\-Gerçek zamanlı sistemlerde determinizmin neden kritik olduğunu sadece teoride değil kendi geliştirdikleri schedulerda deneyimlemek



Soft Skill Kazanımları:

\-Karmaşık bir standardı okuyup mühendislik çözümüne dönüştürme

\-Kısıtlı bütçe ve donanımla problem çözme

\-Dokümantasyon ve sunum

