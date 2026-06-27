import { ShoppingCart, Flame, TrendingUp } from 'lucide-react'
import { useAuth } from '../context/AuthContext'
import { addToCart } from '../api/client'
import toast from 'react-hot-toast'

// demand prop: { predicted_units, restock_needed } — from ML service
// aiDiscount: number — from ML service (admin view only)
export default function JerseyCard({ jersey, onCartUpdate, demand, aiDiscount }) {
  const { user } = useAuth()

  const handleAddToCart = async () => {
    if (!user)               { toast.error('Please login first!'); return }
    if (user.role === 'admin') { toast.error('Admins cannot shop!'); return }
    try {
      const fanId = user.fanId || 101
      await addToCart(fanId, jersey.id, 1)
      toast.success(`${jersey.player} jersey added!`)
      onCartUpdate?.()
    } catch { toast.error('Could not add to cart') }
  }

  const sportEmoji = jersey.sport === 'Football' ? '⚽' : '🏏'
  const stockColor = jersey.stock < 3 ? 'var(--red)'
                   : jersey.stock < 6 ? 'var(--gold)' : 'var(--accent)'

  // ML demand signals
  const isHotItem     = demand?.predicted_units > 25
  const isRestock     = demand?.restock_needed
  const demandUnits   = demand?.predicted_units

  // Effective discount: AI suggested vs jersey's base discount
  const effectiveDiscount = aiDiscount != null
    ? Math.max(jersey.discount || 0, aiDiscount)
    : jersey.discount || 0

  const finalPrice = jersey.price * (1 - effectiveDiscount / 100)

  return (
    <div className="card" style={{ padding: 0, overflow: 'hidden', display: 'flex', flexDirection: 'column', position: 'relative' }}>

      {/* Hot item ribbon */}
      {isHotItem && (
        <div style={{
          position: 'absolute', top: 12, right: 12, zIndex: 2,
          background: 'linear-gradient(135deg, #ff4d4d, #ff8c00)',
          color: '#fff', fontSize: '0.68rem', fontWeight: 700,
          padding: '3px 10px', borderRadius: 999,
          display: 'flex', alignItems: 'center', gap: 4,
          boxShadow: '0 2px 8px rgba(255,77,0,0.4)',
        }}>
          <Flame size={11} /> TRENDING
        </div>
      )}

      {/* Sport color band */}
      <div style={{
        height: 4,
        background: jersey.sport === 'Football'
          ? 'linear-gradient(90deg, var(--blue), #1a6fc4)'
          : 'linear-gradient(90deg, var(--gold), #c48a00)',
      }} />

      <div style={{ padding: '20px', flex: 1, display: 'flex', flexDirection: 'column', gap: 12 }}>

        {/* Badges */}
        <div style={{ display: 'flex', gap: 6, flexWrap: 'wrap' }}>
          <span className={`badge badge-${jersey.sport.toLowerCase()}`}>{sportEmoji} {jersey.sport}</span>
          {jersey.limited && <span className="badge badge-limited">★ Limited</span>}
          {jersey.stock < 5 && <span className="badge badge-low">Low Stock</span>}
        </div>

        {/* Name */}
        <div>
          <h3 style={{ fontFamily: 'var(--font-display)', fontSize: '1.25rem', lineHeight: 1.1 }}>
            {jersey.name}
          </h3>
          <p style={{ fontSize: '0.8rem', color: 'var(--text-secondary)', marginTop: 4 }}>
            {jersey.details}
          </p>
        </div>

        {/* Team + season */}
        <div style={{ display: 'flex', justifyContent: 'space-between' }}>
          <span style={{ fontSize: '0.85rem', color: 'var(--text-secondary)' }}>{jersey.team}</span>
          <span style={{ fontFamily: 'var(--font-mono)', fontSize: '0.75rem', color: 'var(--text-muted)' }}>
            {jersey.season}
          </span>
        </div>

        {/* ML Demand signal — shown to customers naturally */}
        {demandUnits != null && (
          <div style={{
            display: 'flex', alignItems: 'center', gap: 6,
            padding: '6px 10px',
            background: isRestock ? 'rgba(255,77,77,0.08)' : 'rgba(181,255,77,0.06)',
            borderRadius: 'var(--radius-sm)',
            border: `1px solid ${isRestock ? 'rgba(255,77,77,0.2)' : 'rgba(181,255,77,0.15)'}`,
          }}>
            <TrendingUp size={13} color={isRestock ? 'var(--red)' : 'var(--accent)'} />
            <span style={{
              fontSize: '0.75rem', fontWeight: 600,
              color: isRestock ? 'var(--red)' : 'var(--accent)',
            }}>
              {isRestock
                ? `🔥 ${Math.round(demandUnits)} units selling — low supply!`
                : `~${Math.round(demandUnits)} units expected this week`}
            </span>
          </div>
        )}

        {/* Stock bar */}
        <div>
          <div style={{ height: 3, background: 'var(--bg-raised)', borderRadius: 2, overflow: 'hidden' }}>
            <div style={{
              height: '100%',
              width: `${Math.min(jersey.stock * 5, 100)}%`,
              background: stockColor,
              transition: 'width 0.5s ease',
            }} />
          </div>
          <p style={{ fontSize: '0.72rem', color: stockColor, marginTop: 4, fontFamily: 'var(--font-mono)' }}>
            {jersey.stock} in stock
          </p>
        </div>

        <div style={{ flex: 1 }} />

        {/* Price + CTA */}
        <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginTop: 8 }}>
          <div>
            <p style={{ fontFamily: 'var(--font-display)', fontSize: '1.5rem', color: 'var(--accent)', lineHeight: 1 }}>
              ₹{Math.round(finalPrice)}
            </p>
            {effectiveDiscount > 0 && (
              <div style={{ display: 'flex', alignItems: 'center', gap: 4, marginTop: 2 }}>
                <s style={{ fontSize: '0.72rem', color: 'var(--text-muted)' }}>₹{jersey.price}</s>
                <span style={{
                  fontSize: '0.68rem', fontWeight: 700,
                  color: aiDiscount != null ? '#ff8c00' : 'var(--accent)',
                  background: aiDiscount != null ? 'rgba(255,140,0,0.1)' : 'var(--accent-glow)',
                  padding: '1px 6px', borderRadius: 999,
                }}>
                  {aiDiscount != null ? `AI: ${effectiveDiscount.toFixed(0)}% off` : `${effectiveDiscount}% off`}
                </span>
              </div>
            )}
          </div>

          <button className="btn btn-primary" onClick={handleAddToCart}
            disabled={!jersey.inStock}
            style={{ padding: '8px 14px', fontSize: '0.8rem' }}>
            <ShoppingCart size={14} />
            {jersey.inStock ? 'Add' : 'Sold Out'}
          </button>
        </div>
      </div>
    </div>
  )
}
