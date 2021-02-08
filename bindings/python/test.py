from application_network import Appnet,AppnetApplication

appnet = Appnet(b"node95")
appnet.set_application_type()
app = appnet.get_application()
app.add_action(b"1234")
app.add_action(b"5555")
app.add_view(b"python.view")
app.add_view(b"python.view2")
appnet.start()

for i in range(3):
    appnet.recive_event()

appnet.stop()
