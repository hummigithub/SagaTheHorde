// SagaTheHorde Config Builder (static web tool)
// Generates config.json matching version 2.91

(function() {
  const VERSION = "3.0";

  // Default model matching STH_Config constructors and defaults
  function defaultConfig() {
    return orderConfig({
      ConfigVersion: VERSION,
      Global: {
        UseExpansionNotifications: true,
        HordeIntervalSeconds: 1200,
        HordeLifetimeSeconds: 1800,
        DespawnOnServerRestart: true,
        MinPlayersForHordeSpawn: 0,
        SpawnMessageTemplate: "A zombie horde has spawned near {location} ({count})",
        DespawnMessageTemplate: "The zombie horde near {location} has disappeared ({count})",
        MaxActiveHordes: 1,
        TeleportSoundEnabled: true,
        EnableSmoke: true,
        EnableChestReward: true,
        ChestClass: "SeaChest",
        ChestDespawnSeconds: 600,
        RewardPickMin: 1,
        RewardPickMax: 3,
        RewardItems: [
          { ClassName: "Ammo_762x39", QuantityMin: 20, QuantityMax: 40, Chance: 0.8 },
          { ClassName: "BandageDressing", QuantityMin: 1, QuantityMax: 3, Chance: 1.0 }
        ],
        EnableChestSmoke: true,
        ChestSmokeOffsetY: 0.6,
        EnableHordeProgressDisplay: true,
        HordeProgressIntervalSeconds: 10,
        HordeProgressPlayerRadius: 60.0
      },
      Hordes: [
        {
          Name: "Chernogorsk",
          Position: [6740.0, 0.0, 2450.0],
          Radius: 60.0,
          MinCount: 8,
          MaxCount: 15,
          ProgressPlayerRadius: 0.0,
          ZombieClasses: [
            {
              ClassName: "ZmbM_HermitSkinny_Beige",
              Always: false,
              Chance: 0.6,
              CountMin: 1,
              CountMax: 2,
              Loot: ["BandageDressing"],
              TeleportEnabled: true,
              TeleportDetectRadius: 25.0,
              TeleportCooldownSeconds: 15,
              TeleportCombatSuppressRadius: 12.0,
              TeleportMinDistance: 4.0,
              HealthStrengthBonus: 0.5
            },
            {
              ClassName: "ZmbF_CitizenANormal_Beige",
              Always: true,
              Chance: 1.0,
              CountMin: 1,
              CountMax: 3,
              Loot: [],
              TeleportEnabled: false,
              TeleportDetectRadius: 20.0,
              TeleportCooldownSeconds: 20,
              TeleportCombatSuppressRadius: 10.0,
              TeleportMinDistance: 4.0,
              HealthStrengthBonus: 0.0
            }
          ]
        }
      ]
    });
  }

  // Ensure consistent key order in output
  function orderConfig(cfg) {
    const orderRewardItem = (r) => ({
      ClassName: r.ClassName || "",
      QuantityMin: toInt(r.QuantityMin, 1),
      QuantityMax: toInt(r.QuantityMax, 1),
      Chance: toFloat(r.Chance, 1.0)
    });

    const orderZombie = (z) => ({
      ClassName: z.ClassName || "",
      Always: !!z.Always,
      Chance: toFloat(z.Chance, 1.0),
      CountMin: toInt(z.CountMin, 1),
      CountMax: toInt(z.CountMax, 1),
      Loot: Array.isArray(z.Loot) ? z.Loot.filter(Boolean) : [],
      TeleportEnabled: !!z.TeleportEnabled,
      TeleportDetectRadius: toFloat(z.TeleportDetectRadius, 20.0),
      TeleportCooldownSeconds: toInt(z.TeleportCooldownSeconds, 20),
      TeleportCombatSuppressRadius: toFloat(z.TeleportCombatSuppressRadius, 10.0),
      TeleportMinDistance: toFloat(z.TeleportMinDistance, 4.0),
      HealthStrengthBonus: toFloat(z.HealthStrengthBonus, 0.0)
    });

    const orderHorde = (h) => ({
      Name: h.Name || "",
      Position: Array.isArray(h.Position) && h.Position.length === 3
        ? [toFloat(h.Position[0], 0.0), toFloat(h.Position[1], 0.0), toFloat(h.Position[2], 0.0)]
        : [0.0, 0.0, 0.0],
      Radius: toFloat(h.Radius, 25.0),
      MinCount: toInt(h.MinCount, 10),
      MaxCount: toInt(h.MaxCount, 20),
      ProgressPlayerRadius: toFloat(h.ProgressPlayerRadius, 0.0),
      ZombieClasses: (Array.isArray(h.ZombieClasses) ? h.ZombieClasses : []).map(orderZombie)
    });

    const g = cfg.Global || {};
    const ordered = {
      ConfigVersion: String(cfg.ConfigVersion || VERSION),
      Global: {
        UseExpansionNotifications: !!g.UseExpansionNotifications,
        HordeIntervalSeconds: toInt(g.HordeIntervalSeconds, 1200),
        HordeLifetimeSeconds: toInt(g.HordeLifetimeSeconds, 1800),
        DespawnOnServerRestart: !!g.DespawnOnServerRestart,
        MinPlayersForHordeSpawn: toInt(g.MinPlayersForHordeSpawn, 0),
        SpawnMessageTemplate: String(g.SpawnMessageTemplate || "A zombie horde has spawned near {location} ({count})"),
        DespawnMessageTemplate: String(g.DespawnMessageTemplate || "The zombie horde near {location} has disappeared ({count})"),
        MaxActiveHordes: toInt(g.MaxActiveHordes, 1),
        TeleportSoundEnabled: !!g.TeleportSoundEnabled,
        EnableSmoke: !!g.EnableSmoke,
        EnableChestReward: !!g.EnableChestReward,
        ChestClass: String(g.ChestClass || "SeaChest"),
        ChestDespawnSeconds: toInt(g.ChestDespawnSeconds, 600),
        RewardPickMin: toInt(g.RewardPickMin, 1),
        RewardPickMax: toInt(g.RewardPickMax, 3),
        RewardItems: (Array.isArray(g.RewardItems) ? g.RewardItems : []).map(orderRewardItem),
        EnableChestSmoke: !!g.EnableChestSmoke,
        ChestSmokeOffsetY: toFloat(g.ChestSmokeOffsetY, 0.6),
        EnableHordeProgressDisplay: !!g.EnableHordeProgressDisplay,
        HordeProgressIntervalSeconds: toInt(g.HordeProgressIntervalSeconds, 10),
        HordeProgressPlayerRadius: toFloat(g.HordeProgressPlayerRadius, 60.0)
      },
      Hordes: (Array.isArray(cfg.Hordes) ? cfg.Hordes : []).map(orderHorde)
    };
    // RewardPickMin must be <= RewardPickMax
    if (ordered.Global.RewardPickMin > ordered.Global.RewardPickMax) {
      ordered.Global.RewardPickMin = ordered.Global.RewardPickMax;
    }
    return ordered;
  }

  function toInt(v, d) { const n = parseInt(v, 10); return isNaN(n) ? d : n; }
  function toFloat(v, d) { const n = parseFloat(v); return isNaN(n) ? d : n; }

  // State
  let state = defaultConfig();

  // UI rendering helpers
  const $ = (sel) => document.querySelector(sel);
  const $$ = (sel) => Array.from(document.querySelectorAll(sel));

  function render() {
    renderGlobal();
    renderRewards();
    renderHordes();
    updatePreview();
  }

  function bindInput(el, obj, key, parser = (v)=>v) {
    el.value = obj[key] !== undefined ? obj[key] : "";
    el.addEventListener('input', () => { obj[key] = parser(el.value); updatePreview(); });
  }
  function bindCheckbox(el, obj, key) {
    el.checked = !!obj[key];
    el.addEventListener('change', () => { obj[key] = !!el.checked; updatePreview(); });
  }

  function renderGlobal() {
    const g = state.Global;
    const root = $('#global-form');
    root.innerHTML = '';
    root.appendChild(rowBool('Use Expansion Notifications', g, 'UseExpansionNotifications'));
    root.appendChild(rowNum('Horde Interval (sec)', g, 'HordeIntervalSeconds', true));
    root.appendChild(rowNum('Horde Lifetime (sec)', g, 'HordeLifetimeSeconds', true));
    root.appendChild(rowBool('Despawn On Server Restart', g, 'DespawnOnServerRestart'));
    root.appendChild(rowNum('Min Players For Spawn', g, 'MinPlayersForHordeSpawn', true));
    root.appendChild(rowText('Spawn Message Template', g, 'SpawnMessageTemplate'));
    root.appendChild(rowText('Despawn Message Template', g, 'DespawnMessageTemplate'));
    root.appendChild(rowNum('Max Active Hordes', g, 'MaxActiveHordes', true));
    root.appendChild(rowBool('Teleport Sound Enabled', g, 'TeleportSoundEnabled'));
    root.appendChild(rowBool('Enable Smoke', g, 'EnableSmoke'));
    root.appendChild(rowBool('Enable Chest Reward', g, 'EnableChestReward'));
    root.appendChild(rowText('Chest Class', g, 'ChestClass'));
  root.appendChild(rowNum('Chest Despawn (sec, 0=off)', g, 'ChestDespawnSeconds', true));
    root.appendChild(rowNum('Reward Pick Min', g, 'RewardPickMin', true));
    root.appendChild(rowNum('Reward Pick Max', g, 'RewardPickMax', true));
    root.appendChild(rowBool('Enable Chest Smoke', g, 'EnableChestSmoke'));
    root.appendChild(rowNum('Chest Smoke Offset Y', g, 'ChestSmokeOffsetY', false));
    root.appendChild(rowBool('Enable Horde Progress Display', g, 'EnableHordeProgressDisplay'));
    root.appendChild(rowNum('Horde Progress Interval (sec)', g, 'HordeProgressIntervalSeconds', true));
    root.appendChild(rowNum('Horde Progress Player Radius (m)', g, 'HordeProgressPlayerRadius', false));
  }

  function rowText(label, obj, key) {
    const wrap = document.createElement('div');
    wrap.className = 'form-row';
    wrap.innerHTML = `<label>${label}</label>`;
    const input = document.createElement('input');
    input.type = 'text';
    bindInput(input, obj, key, (v)=>v);
    wrap.appendChild(input);
    const spacer1 = document.createElement('div');
    const spacer2 = document.createElement('div');
    wrap.appendChild(spacer1); wrap.appendChild(spacer2);
    return wrap;
  }
  function rowNum(label, obj, key, integer=false) {
    const wrap = document.createElement('div'); wrap.className = 'form-row';
    wrap.innerHTML = `<label>${label}</label>`;
    const input = document.createElement('input'); input.type = 'number'; input.step = integer ? '1' : '0.1';
    bindInput(input, obj, key, (v)=> integer ? toInt(v, 0) : toFloat(v, 0));
    wrap.appendChild(input);
    const spacer1 = document.createElement('div');
    const spacer2 = document.createElement('div');
    wrap.appendChild(spacer1); wrap.appendChild(spacer2);
    return wrap;
  }
  function rowBool(label, obj, key) {
    const wrap = document.createElement('div'); wrap.className = 'form-row';
    wrap.innerHTML = `<label>${label}</label>`;
    const input = document.createElement('input'); input.type = 'checkbox';
    bindCheckbox(input, obj, key);
    wrap.appendChild(input);
    const spacer1 = document.createElement('div');
    const spacer2 = document.createElement('div');
    wrap.appendChild(spacer1); wrap.appendChild(spacer2);
    return wrap;
  }

  function renderRewards() {
    const list = $('#rewards-list'); list.innerHTML = '';
    const items = state.Global.RewardItems || [];
    items.forEach((r, idx) => {
      const item = document.createElement('div'); item.className = 'item';
      const title = document.createElement('div'); title.className = 'title';
      title.innerHTML = `<span>Reward #${idx+1}</span>`;
      const btns = document.createElement('div'); btns.className = 'btns';
      const delBtn = button('Delete', () => { items.splice(idx,1); renderRewards(); updatePreview(); });
      btns.appendChild(delBtn); title.appendChild(btns); item.appendChild(title);

      const row = document.createElement('div'); row.className = 'row';
      row.appendChild(labelEl('ClassName')); row.appendChild(textInput(r, 'ClassName'));
      row.appendChild(labelEl('QuantityMin')); row.appendChild(numInput(r, 'QuantityMin', true));
      row.appendChild(labelEl('QuantityMax')); row.appendChild(numInput(r, 'QuantityMax', true));
      row.appendChild(labelEl('Chance')); row.appendChild(numInput(r, 'Chance', false));
      item.appendChild(row);

      list.appendChild(item);
    });
  }

  function renderHordes() {
    const root = $('#hordes-list'); root.innerHTML = '';
    const hordes = state.Hordes || [];
    hordes.forEach((h, idx) => {
      const wrap = document.createElement('div'); wrap.className = 'item';
      const title = document.createElement('div'); title.className = 'title';
      const nameInput = document.createElement('input'); nameInput.type = 'text'; nameInput.placeholder = 'Horde name';
      bindInput(nameInput, h, 'Name', (v)=>v);
      title.appendChild(nameInput);
      const btns = document.createElement('div'); btns.className = 'btns';
      btns.appendChild(button('Delete', () => { hordes.splice(idx,1); renderHordes(); updatePreview(); }));
      btns.appendChild(button('Duplicate', () => { hordes.splice(idx+1,0, JSON.parse(JSON.stringify(h))); renderHordes(); updatePreview(); }));
      title.appendChild(btns);
      wrap.appendChild(title);

      const g1 = document.createElement('div'); g1.className = 'group';
      g1.innerHTML = '<h4>Location</h4>';
      const row1 = document.createElement('div'); row1.className = 'row';
      row1.appendChild(labelEl('Position X')); row1.appendChild(numInputArray(h, 'Position', 0, false));
      row1.appendChild(labelEl('Position Y')); row1.appendChild(numInputArray(h, 'Position', 1, false));
      row1.appendChild(labelEl('Position Z')); row1.appendChild(numInputArray(h, 'Position', 2, false));
      row1.appendChild(labelEl('Radius')); row1.appendChild(numInput(h, 'Radius', false));
      row1.appendChild(labelEl('MinCount')); row1.appendChild(numInput(h, 'MinCount', true));
      row1.appendChild(labelEl('MaxCount')); row1.appendChild(numInput(h, 'MaxCount', true));
      row1.appendChild(labelEl('ProgressPlayerRadius')); row1.appendChild(numInput(h, 'ProgressPlayerRadius', false));
      g1.appendChild(row1);
      wrap.appendChild(g1);

      const g2 = document.createElement('div'); g2.className = 'group'; g2.innerHTML = '<h4>Zombie Classes</h4>';
      const zList = document.createElement('div'); zList.className = 'list';
      (h.ZombieClasses || []).forEach((z, zi) => {
        const zItem = document.createElement('div'); zItem.className = 'item';
        const zTitle = document.createElement('div'); zTitle.className = 'title';
        const zLabel = document.createElement('span'); zLabel.textContent = `Zombie #${zi+1}`; zTitle.appendChild(zLabel);
        const zBtns = document.createElement('div'); zBtns.className = 'btns';
        zBtns.appendChild(button('Delete', () => { h.ZombieClasses.splice(zi,1); renderHordes(); updatePreview(); }));
        zBtns.appendChild(button('Duplicate', () => { h.ZombieClasses.splice(zi+1,0, JSON.parse(JSON.stringify(z))); renderHordes(); updatePreview(); }));
        zTitle.appendChild(zBtns); zItem.appendChild(zTitle);

        const base = document.createElement('div'); base.className = 'row';
        base.appendChild(labelEl('ClassName')); base.appendChild(textInput(z, 'ClassName'));
        base.appendChild(labelEl('Always')); base.appendChild(checkboxInput(z, 'Always'));
        base.appendChild(labelEl('Chance')); base.appendChild(numInput(z, 'Chance', false));
        base.appendChild(labelEl('CountMin')); base.appendChild(numInput(z, 'CountMin', true));
        base.appendChild(labelEl('CountMax')); base.appendChild(numInput(z, 'CountMax', true));
        base.appendChild(labelEl('HealthStrengthBonus')); base.appendChild(numInput(z, 'HealthStrengthBonus', false));
        zItem.appendChild(base);

        const tele = document.createElement('div'); tele.className = 'row';
        tele.appendChild(labelEl('TeleportEnabled')); tele.appendChild(checkboxInput(z, 'TeleportEnabled'));
        tele.appendChild(labelEl('TeleportDetectRadius')); tele.appendChild(numInput(z, 'TeleportDetectRadius', false));
        tele.appendChild(labelEl('TeleportCooldownSeconds')); tele.appendChild(numInput(z, 'TeleportCooldownSeconds', true));
        tele.appendChild(labelEl('TeleportCombatSuppressRadius')); tele.appendChild(numInput(z, 'TeleportCombatSuppressRadius', false));
        tele.appendChild(labelEl('TeleportMinDistance')); tele.appendChild(numInput(z, 'TeleportMinDistance', false));
        zItem.appendChild(tele);

        const loot = document.createElement('div'); loot.className = 'row';
        loot.appendChild(labelEl('Loot'));
        const lootWrap = document.createElement('div'); lootWrap.style.gridColumn = 'span 3';
        const lootList = document.createElement('div'); lootList.style.display = 'flex'; lootList.style.flexWrap = 'wrap'; lootList.style.gap = '6px';
        (z.Loot || []).forEach((l, li) => {
          const pill = document.createElement('span'); pill.textContent = l; pill.style.border = '1px solid #ddd'; pill.style.borderRadius = '999px'; pill.style.padding = '2px 8px';
          const del = document.createElement('button'); del.textContent = 'x'; del.className = 'small'; del.style.marginLeft = '6px';
          const wrapPill = document.createElement('span'); wrapPill.appendChild(pill); wrapPill.appendChild(del);
          del.addEventListener('click', ()=>{ z.Loot.splice(li,1); renderHordes(); updatePreview();});
          lootList.appendChild(wrapPill);
        });
        const addLootInput = document.createElement('input'); addLootInput.type = 'text'; addLootInput.placeholder = 'Add loot class and press Enter';
        addLootInput.addEventListener('keydown', (e)=>{
          if (e.key === 'Enter') { const v = addLootInput.value.trim(); if (v) { z.Loot = z.Loot || []; z.Loot.push(v); addLootInput.value=''; renderHordes(); updatePreview(); }}
        });
        lootWrap.appendChild(lootList); lootWrap.appendChild(addLootInput);
        loot.appendChild(lootWrap);
        zItem.appendChild(loot);

        zList.appendChild(zItem);
      });
      const addZ = button('+ Add Zombie Class', () => {
        h.ZombieClasses = h.ZombieClasses || [];
        h.ZombieClasses.push({
          ClassName: "",
          Always: false,
          Chance: 1.0,
          CountMin: 1,
          CountMax: 1,
          Loot: [],
          TeleportEnabled: false,
          TeleportDetectRadius: 20.0,
          TeleportCooldownSeconds: 20,
          TeleportCombatSuppressRadius: 10.0,
          TeleportMinDistance: 4.0,
          HealthStrengthBonus: 0.0
        });
        renderHordes(); updatePreview();
      });
      g2.appendChild(zList); g2.appendChild(addZ);
      wrap.appendChild(g2);

      root.appendChild(wrap);
    });
  }

  function labelEl(text) { const l = document.createElement('label'); l.textContent = text; return l; }
  function button(text, onClick) { const b = document.createElement('button'); b.textContent = text; b.type = 'button'; b.addEventListener('click', onClick); return b; }
  function textInput(obj, key) { const i = document.createElement('input'); i.type = 'text'; bindInput(i, obj, key, (v)=>v); return i; }
  function numInput(obj, key, integer=false) { const i = document.createElement('input'); i.type = 'number'; i.step = integer ? '1' : '0.1'; bindInput(i, obj, key, (v)=> integer ? toInt(v, 0) : toFloat(v, 0)); return i; }
  function checkboxInput(obj, key) { const i = document.createElement('input'); i.type = 'checkbox'; bindCheckbox(i, obj, key); return i; }
  function numInputArray(obj, key, idx, integer=false) { const i = document.createElement('input'); i.type = 'number'; i.step = integer ? '1' : '0.1'; i.value = Array.isArray(obj[key]) ? obj[key][idx] : 0; i.addEventListener('input', ()=>{ obj[key] = obj[key] || [0,0,0]; obj[key][idx] = integer ? toInt(i.value, 0) : toFloat(i.value, 0); updatePreview();}); return i; }

  function updatePreview() {
    const ordered = orderConfig(state);
    $('#preview').value = JSON.stringify(ordered, null, 2);
  }

  // Import/export
  $('#file-import').addEventListener('change', async (e) => {
    const file = e.target.files[0]; if (!file) return;
    try {
      const text = await file.text();
      const json = JSON.parse(text);
      // Force version to target, but keep user's content
      json.ConfigVersion = VERSION;
      state = orderConfig(json);
      render();
    } catch (err) { alert('Import failed: ' + err.message); }
    e.target.value = '';
  });

  $('#btn-export').addEventListener('click', () => {
    const data = JSON.stringify(orderConfig(state), null, 2);
    const blob = new Blob([data], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url; a.download = 'config.json';
    document.body.appendChild(a); a.click(); document.body.removeChild(a);
    URL.revokeObjectURL(url);
  });

  $('#btn-copy').addEventListener('click', async () => {
    try { await navigator.clipboard.writeText(JSON.stringify(orderConfig(state), null, 2)); alert('JSON copied to clipboard'); }
    catch { alert('Copy failed.'); }
  });

  $('#btn-new').addEventListener('click', () => { state = defaultConfig(); render(); });
  $('#btn-add-reward').addEventListener('click', () => { state.Global.RewardItems.push({ ClassName: '', QuantityMin: 1, QuantityMax: 1, Chance: 1.0 }); renderRewards(); updatePreview(); });
  $('#btn-add-horde').addEventListener('click', () => {
    state.Hordes.push({
      Name: '', Position: [0,0,0], Radius: 25.0, MinCount: 10, MaxCount: 20, ProgressPlayerRadius: 0.0, ZombieClasses: []
    });
    renderHordes(); updatePreview();
  });

  // Initial render
  render();
})();
