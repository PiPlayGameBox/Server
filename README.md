# Serveri çalıştırmak için bu kütüphanelerin kurulması gerekli
# sudo apt-get install libssl-dev
# sudo yum install openssl-devel

# Ondan sonra make run diyerek server çalıştırabilir
# Config.txt dosyasındaki i.p adresi, kullandığınız bilgisayarın i.p adresi olmalı
# Server programını ctr + c ile kapatmak gerekli ordaki handler socket'i kapatıyor
# Server kapandıktan sonra bazen bir daha açmak istendiği zaman hata alınabilir operating system
# tam kapatmayabiliyor, orda port numarası arttılarak devam edilebilir, gerçek projede server'i kapatmayacağımız
# için böyle bir durum olmayacak