cd core
uglifyjs Disjunction.js App.js Builder.js Ctrl.js Device.js DeviceChannel.js DeviceHub.js Model.js Phase.js Phaser.js Point2.js Pointer.js ServiceHub.js Timeline.js Timer.js View.js -o ../build/disjunction-core.min.js
cd ..
REM --source-map ../build/disjunction-core.min.js.map
